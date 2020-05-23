#pragma once

#include <upcxx/upcxx.hpp>
#include "kmer_t.hpp"

struct HashMap {
	// 	std::vector <kmer_pair> data;
	// 	std::vector <int> used;

	//global vectors
	/**LOCAL ARRAYS NEEDED TO BE ABLE TO DO ARRAY LOGIC (eg "arr[i]" or "arr + itr")**/
	std::vector< upcxx::global_ptr<kmer_pair> > global_data;
	std::vector< upcxx::global_ptr<int> > global_used;

	size_t my_size;
	size_t size_n;

	size_t size() const noexcept;
	size_t global_size() const noexcept;

	HashMap(size_t size);

	// Most important functions: insert and retrieve
	// k-mers from the hash table.
	bool insert(const kmer_pair& kmer);
	bool find(const pkmer_t& key_kmer, kmer_pair& val_kmer);

	// Helper functions
	int find_rank(uint64_t slot);

	// Write and read to a logical data slot in the table.
	void write_slot(uint64_t slot, const kmer_pair& kmer);
	kmer_pair read_slot(uint64_t slot);

	// Request a slot or check if it's already used.
	bool request_slot(uint64_t slot);
	bool slot_used(uint64_t slot);
};

HashMap::HashMap(size_t size) {
	/*my_size = size;
	data.resize(size);
	used.resize(size, 0);*/

	// Resize global arrays
	global_data.resize(upcxx::rank_n());
	global_used.resize(upcxx::rank_n());

	// Global size variables
	size_n = size;
	my_size = (size + upcxx::rank_n() - 1) / upcxx::rank_n();

	//each proc allocates its pointer on the array
	global_data[upcxx::rank_me()] = upcxx::new_array<kmer_pair>(my_size);
	global_used[upcxx::rank_me()] = upcxx::new_array<int>(my_size);

	//each processor broadcasts its ptr
	for (int i = 0; i < upcxx::rank_n(); i++)
	{
		global_data[i] = upcxx::broadcast(global_data[i], i).wait();
		global_used[i] = upcxx::broadcast(global_used[i], i).wait();
	}
}

bool HashMap::insert(const kmer_pair& kmer) {
	uint64_t hash = kmer.hash();
	uint64_t probe = 0;
	bool success = false;
	do {
		uint64_t slot = (hash + probe++) % global_size();
		success = request_slot(slot);
		if (success) {
			write_slot(slot, kmer);
		}
	} while (!success && probe < global_size());
	return success;
}

bool HashMap::find(const pkmer_t& key_kmer, kmer_pair& val_kmer) {
	uint64_t hash = key_kmer.hash();
	uint64_t probe = 0;
	bool success = false;
	do {
		uint64_t slot = (hash + probe++) % global_size();
		if (slot_used(slot)) {
			val_kmer = read_slot(slot);
			if (val_kmer.kmer == key_kmer) {
				success = true;
			}
		}
	} while (!success && probe < global_size());
	return success;
}

bool HashMap::slot_used(uint64_t slot) {
	//return used[slot] != 0;

	int my_rank = find_rank(slot);

	//future to handle data races
	upcxx::future<int> used_future = rget(global_used[my_rank] + slot % my_size);
	used_future.wait();

	assert((global_used[my_rank] + slot % my_size).is_local());
	int* local_used = (global_used[my_rank] + slot % my_size).local();

	return *local_used != 0;


}

void HashMap::write_slot(uint64_t slot, const kmer_pair& kmer) {
	//data[slot] = kmer;

	int my_rank = find_rank(slot);

	upcxx::rput(kmer, global_data[my_rank] + slot % my_size).wait();

}

kmer_pair HashMap::read_slot(uint64_t slot) {
	//return data[slot];

	int my_rank = find_rank(slot);

	//future to handle data races
	upcxx::future<kmer_pair> data_future = rget(global_data[my_rank] + slot % my_size);
	data_future.wait();

	assert((global_data[my_rank] + slot % my_size).is_local());
	kmer_pair* local_data = (global_data[my_rank] + slot % my_size).local();

	return *local_data;

}

bool HashMap::request_slot(uint64_t slot) {
	int my_rank = find_rank(slot);

	//future to handle data races
	upcxx::future<int> used_future = rget(global_used[my_rank] + slot % my_size);
	used_future.wait();

	assert((global_used[my_rank] + slot % my_size).is_local());
	int* local_used = (global_used[my_rank] + slot % my_size).local();

	if (*local_used != 0) {
		return false;
	}
	else {
		*local_used = 1;
		return true;
	}
}

//function that returns the current rank size
size_t HashMap::size() const noexcept {
	return my_size;
}

//function that returns the global size
size_t HashMap::global_size() const noexcept
{
	return size_n;
}

//function that finds the specified rank
int HashMap::find_rank(uint64_t slot)
{
	return int(slot / my_size);
}