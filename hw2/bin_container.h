#ifndef	BIN_CONTAINER_H
#define BIN_CONTAINER_H

#include "common.h"
#include <vector>

#define CUTOFF 0.01
#define DENSITY 0.0005
class BinContainer
{
public:
	BinContainer(int numParticles);

	void buildBins(particle_t* particles, int n);
    std::vector<particle_t>* returnArrayRowPtr(int row);
	int getVectorSize();
	int getNumBins();

private:
	// internal vector holds particles in each bin cell
	std::vector< std::vector<particle_t> > bins;
	double gridSize;
	double binSize;
	int numBins;
};


#endif // BIN_PARTICLE_