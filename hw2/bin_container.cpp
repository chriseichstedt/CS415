#include "bin_container.h"
#include <cmath>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

BinContainer::BinContainer(int numParticles)
{
	gridSize = sqrt(numParticles * DENSITY);
	binSize = 2 * CUTOFF;
	numBins = int(gridSize/binSize) + 1;
}

void BinContainer::buildBins(particle_t* particles, int n)
{
	bins.resize(numBins*numBins);

	for (int i = 0; i < n; ++i) 
	{
		int x = int(particles[i].x / binSize);
		int y = int(particles[i].y / binSize);
		bins[x*numBins+y].push_back(particles[i]);
	}
}

std::vector<particle_t>* BinContainer::returnArrayRowPtr(int row)
{
//     std::vector<particle_t>* temp = (particle_t*)malloc(row*numBins*sizeof(particle_t));
//     temp = &bins[row*numBins];
// 	return &((*temp)[0]);

    // std::cout << bins[row*numBins].at(0).x << std::endl;
    return &bins[row*numBins];
}

int BinContainer::getVectorSize()
{
	return bins.size();
}

int BinContainer::getNumBins()
{
    return numBins;
}