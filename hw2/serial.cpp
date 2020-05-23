#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "common.h"
#include <vector>
#include <algorithm>
#include <iostream>

using std::vector;

/**psuedo-code
 * loop through time ticks
 *  loop through bins
 *      loop through particles[i]
 *          loop through particles[j]
 *              loop through particles in neighboring bins
 *                  appleForce()
 *  Barrier
 * 
 * Loop through particles
 *  move()
 * 
 * Loop through particles
 *  adjustBins()
 **/
 
/**resources
* https://www.geeksforgeeks.org/bucket-sort-2/
* http://bebop.cs.berkeley.edu/bootcamp2013/assignments/index.html
* https://rustie.xyz/2019/03/08/nbody/
**/

//constants need for binning
#define CUTOFF 0.01
#define DENSITY 0.0005

//variables needed for binning
double binSize, gridSize;
int binNum;
vector<particle_t> particlesToChangeBins;

//Vector stuff
// typedef vector<particle_t> bin_t;

//function to build the bins, and store the particles in them based on their coordinates
void buildBins(particle_t* particles, vector< vector<particle_t> > &bins, int n);
int getGridCoord(int size, double x, double y);
int findArrayElement(vector<particle_t> &vec, particle_t &val);

//
//  benchmarking program
//
int main( int argc, char **argv )
{    
    int navg,nabsavg=0;
    double davg,dmin, absmin=1.0, absavg=0.0;

    if( find_option( argc, argv, "-h" ) >= 0 )
    {
        printf( "Options:\n" );
        printf( "-h to see this help\n" );
        printf( "-n <int> to set the number of particles\n" );
        printf( "-o <filename> to specify the output file name\n" );
        printf( "-s <filename> to specify a summary file name\n" );
        printf( "-no turns off all correctness checks and particle output\n");
        return 0;
    }
    
    int n = read_int( argc, argv, "-n", 1000 );

    char *savename = read_string( argc, argv, "-o", NULL );
    char *sumname = read_string( argc, argv, "-s", NULL );
    
    FILE *fsave = savename ? fopen( savename, "w" ) : NULL;
    FILE *fsum = sumname ? fopen ( sumname, "a" ) : NULL;

    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
    set_size( n );
    init_particles( n, particles );
    
    
    //Create bins
    vector< vector<particle_t> > particleBins;
    buildBins(particles, particleBins, n);
    
    //
    //  simulate a number of time steps
    //
    double simulation_time = read_timer( );
	
    for( int step = 0; step < NSTEPS; step++ ) //loop through time ticks
    {
        //constants needed for forces
	    navg = 0;
        davg = 0.0;
	    dmin = 1.0;
        //
        //  compute forces
        //

        for(int x = 0; x < binNum; x++)//loop through 2d bins
        {

            for(int y = 0; y < binNum; y++)
            {
                vector<particle_t>& binXY = particleBins[x*binNum+y]; //store all particles for binXY
                
                //Clear forces of particles in binXY
                for(int i = 0; i < binXY.size(); i++)//set the particles accelerations to 0
                {
                    binXY[i].ax = binXY[i].ay = 0;
                }
                
                //Iterate through bins and search around its neighbors
                for(int deltaX = -1; deltaX <= 1; deltaX++) //x + dx = neighbor left/right
                {
                    for(int deltaY = -1; deltaY <= 1; deltaY++)//y + dy = neighbor top/bottom
                    {
                        if(x + deltaX >= 0) //If the bin is on the left side, can't search outside left boundary
                        {
                            if(x + deltaX < binNum)//If the bin is on the right side, can't search outside right boundary
                            {
                                if(y + deltaY >= 0)//If the bin is on the top side, can't search outside top boundary
                                {
                                    if(y + deltaY < binNum)//If the bin is on the bottom side, can't search outside bottom boundary
                                    {
                                        vector<particle_t>& neighborBin = particleBins[(x + deltaX) * binNum + y + deltaY];//Store all particles for neighbor bin
                                        
                                        for(int i = 0; i < binXY.size(); i++)//for all particles in binXY
                                        {
                                            for(int j = 0; j < neighborBin.size(); j++)//for all particles in neighbor bin
                                            {
                                                apply_force(binXY[i], neighborBin[j], &dmin, &davg, &navg);//Apply proper forces to each particles
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        

        
        //loop through bins
        for(int i=0 ; i<binNum ; i++)//loop through 2d bins
        {
            for(int j=0 ; j<binNum ; j++)
            {
                // check to see if particles in bin are still in the same bin. If they are not move them to new bin
                vector<particle_t>& currentBin = particleBins[i * binNum + j];//get current bin
                int end = currentBin.size(), k=0; //end of the bin and iterator k
                
                while(k<end)//from start to end
                {
                    move(currentBin[k]);//move
                    
                    //check grid coords
                    int xCoord = int(currentBin[k].x/binSize);
                    int yCoord = int(currentBin[k].y/binSize);
                    
                    /**if bin has changed**/
                    
                    //remove from current bin
                    if(xCoord == i && yCoord == j)
                        k++; //keep particle in current bin
                        
                    else
                    {
                        particlesToChangeBins.push_back(currentBin[k]);//add to vector of particles that need to change bins
                        currentBin[k] = currentBin[--end];//remove from current bin
                    }
                    
                }
                currentBin.resize(k);//resize current bin to adhere to changes
            }
        }
        
        //add all particles that moved bins into thier new bins
        for(int i=0 ; i<particlesToChangeBins.size() ; i++)//add all particles that need to change bins to their new bins
        {
            //same code from buildBins()
            int xCoord = int(particlesToChangeBins[i].x/binSize);
            int yCoord = int(particlesToChangeBins[i].y/binSize);
            
            particleBins[xCoord * binNum + yCoord].push_back(particlesToChangeBins[i]);
        }
        particlesToChangeBins.clear();
 
        //
        //  move particles
        //
        // for( int i = 0; i < n; i++ )
        // {
        //     // int gridCoord = getGridCoord(gridSize, particles[i].x, particles[i].y);
        //     move( particles[i] );
            
        // }
        
        if( find_option( argc, argv, "-no" ) == -1 )
        {
          //
          // Computing statistical data
          //
          if (navg) {
            absavg +=  davg/navg;
            nabsavg++;
          }
          if (dmin < absmin) absmin = dmin;
		
          //
          //  save if necessary
          //
          if( fsave && (step%SAVEFREQ) == 0 )
              save( fsave, n, particles );
        }
    }
    
    
    simulation_time = read_timer( ) - simulation_time;
    
    printf( "n = %d, simulation time = %g seconds", n, simulation_time);

    if( find_option( argc, argv, "-no" ) == -1 )
    {
      if (nabsavg) absavg /= nabsavg;
    // 
    //  -the minimum distance absmin between 2 particles during the run of the simulation
    //  -A Correct simulation will have particles stay at greater than 0.4 (of cutoff) with typical values between .7-.8
    //  -A simulation were particles don't interact correctly will be less than 0.4 (of cutoff) with typical values between .01-.05
    //
    //  -The average distance absavg is ~.95 when most particles are interacting correctly and ~.66 when no particles are interacting
    //
    printf( ", absmin = %lf, absavg = %lf", absmin, absavg);
    if (absmin < 0.4) printf ("\nThe minimum distance is below 0.4 meaning that some particle is not interacting");
    if (absavg < 0.8) printf ("\nThe average distance is below 0.8 meaning that most particles are not interacting");
    }
    printf("\n");     

    //
    // Printing summary data
    //
    if( fsum) 
        fprintf(fsum,"%d %g\n",n,simulation_time);
 
    //
    // Clearing space
    //
    if( fsum )
        fclose( fsum );    
    free( particles );
    if( fsave )
        fclose( fsave );
    
    return 0;
}

//Function to build bins for particles
void buildBins(particle_t* particles, vector< vector<particle_t> > &bins, int n)
{
    gridSize = sqrt(n * DENSITY); //the size of the grid of bins
    binSize = CUTOFF * 2; //the size needed for each bin
    binNum = int(gridSize/binSize) + 1;//the number of bins given the size of the grid and the size of the bins
    
    bins.resize(binNum*binNum);//size the vector to be the size of the grid^2
    
    //put the particles into bins based on their coordinates
    for(int i=0 ; i<n ; i++)
    {
        //find the coordinates of the particle approximated to the closest bin to it
        int xCoord = int(particles[i].x/binSize);
        int yCoord = int(particles[i].y/binSize);
        
        //push it to the appropriate bin
        bins[xCoord*binNum+yCoord].push_back(particles[i]);
    }
}

//Function to find current bin coordinates
int getGridCoord(int size, double x, double y)
{
    return ((int) floor(x/binSize))*size+((int) floor(y/binSize));
}

//Function to find if an element exists in an array
int findArrayElement(vector<particle_t> &vec, particle_t &val)
{
    int itr = -1;
    for(int i=0 ; i<vec.size() ; i++)
    {
        // std::cout << vec[i].x << " " << vec[i].y << std::endl;
        if(val.x == vec[i].x && val.y == vec[i].y)
        {
            itr = i;
            break;
		}
	}

    return itr;
}