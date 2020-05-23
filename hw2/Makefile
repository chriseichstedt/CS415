SHELL='bash'
#
# Bridges - PSC
#
# Intel Compilers are loaded by default
# You will need to specifically switch to GNU Modules
# With with `modules.sh`
#

CC = g++
MPCC = mpicxx
OPENMP = -fopenmp
CFLAGS = -O3
LIBS =


TARGETS = serial pthreads openmp mpi autograder mpi_send_recieve

all:	$(TARGETS)

serial: serial.o common.o
	$(CC) -o $@ $(LIBS) serial.o common.o
autograder: autograder.o common.o
	$(CC) -o $@ $(LIBS) autograder.o common.o
pthreads: pthreads.o common.o
	$(CC) -o $@ $(LIBS) -lpthread pthreads.o common.o
openmp: openmp.o common.o
	$(CC) -o $@ $(LIBS) $(OPENMP) openmp.o common.o
mpi: mpi.o common.o bin_container.o
	$(MPCC) -o $@ $(LIBS) $(MPILIBS) mpi.o common.o bin_container.o
mpi_send_recieve: mpi_send_recieve.o common.o
	$(MPCC) -o $@ $(LIBS) $(MPILIBS) mpi_send_recieve.o common.o

autograder.o: autograder.cpp common.h
	$(CC) -c $(CFLAGS) autograder.cpp
openmp.o: openmp.cpp common.h
	$(CC) -c $(OPENMP) $(CFLAGS) openmp.cpp
serial.o: serial.cpp common.h
	$(CC) -c $(CFLAGS) serial.cpp
pthreads.o: pthreads.cpp common.h
	$(CC) -c $(CFLAGS) pthreads.cpp
mpi.o: mpi.cpp common.h
	$(MPCC) -c $(CFLAGS) mpi.cpp
common.o: common.cpp common.h
	$(CC) -c $(CFLAGS) common.cpp
mpi_send_recieve.o: mpi_send_recieve.cpp common.h
	$(MPCC) -c $(CFLAGS) mpi_send_recieve.cpp
bin_container.o: bin_container.h bin_container.cpp
	$(CC) -c $(CFLAGS) bin_container.cpp
	
clean:
	rm -f *.o $(TARGETS) *.stdout *.txt
