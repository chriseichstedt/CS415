#include <mpi.h> 
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include "common.h"
#include <vector>
#include <time.h>

using std::vector;

/**
 * Possible reason for seg fault when sending the row of bins
 * 1) PARTCLE data type for mpi not proper for the sending of the array of bins
 ** did we need an new datatype; what would it be?
 * 
 * 2)Data stored into the recvBuffer was incorrect
 ** was it the send? the data before send was correct
**/

typedef vector<particle_t> bin_t;

particle_t recvPart;
bin_t recvBin;
particle_t* recvRow; 

particle_t* convert2DArrayto1D(particle_t**, int , int);

int* convert2DIntArrayto1D(int** arr, int rows , int cols);


int* testGrid[5];

int main(int argc, char* argv[])
{
  int numtasks, rank, dest, source, rc, count, tag=1;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
    
  char inmsg, outmsg='x';
  MPI_Status Stat;
  MPI_Request Req;

    MPI_Datatype PARTICLE;
    MPI_Type_contiguous( 6, MPI_DOUBLE, &PARTICLE );
    MPI_Type_commit( &PARTICLE );


 
  //create an array of particles
  bin_t arr[10];
  particle_t temp;
  
  srand(time(NULL));
  
  for(int i = 0 ; i<5 ; i++)
    testGrid[i] = (int*)malloc(5*sizeof(int));
  
  for(int i=0 ; i<5 ; i++)
  {
      for(int j=0 ; j<5 ; j++)
        testGrid[i][j] = rand() % 10;
  }
  
//   for(int i=0 ; i<5 ; i++)
//   {
//       for(int j=0 ; j<5 ; j++)
//         std::cout << testGrid[i][j] << " | ";
//     std::cout << std::endl;
//   }


  
  for(int i=0 ; i<10 ; i++)
  {
      srand(time(NULL));
      float xVal = (float)(rand( )% 100);
      float yVal = (float)(rand() % 100);
      temp.x = xVal;
      temp.y = yVal;
      temp.ax = temp.ay = temp.vx = temp.vy = 0.0;
      
      arr[i].push_back(temp);
  }
  
//   for(int i=0 ; i<10 ; i++)
//     std::cout << arr[i].at(0).x << " " << arr[i].at(0).y << std::endl;


  if(rank == 0)
  { 
    //**send single particle
    // MPI_Send(&(arr[0].at(0)), 1, PARTICLE, 1, 0, MPI_COMM_WORLD);
    // std::cout << "Processor 0 Sent: " << (arr[0].at(0)).x << " " << (arr[0].at(0)).y << std::endl;

    // std::cout << "Processor 0 Sent: ";
    // for(int j=0 ; j<1 ; j++)
    //     std::cout << (arr[0].at(j)).x << " " << (arr[0].at(j)).y << " | ";
    // std::cout << std::endl;

    // // **sends a bin(loop method)
    // int sendSize = arr[0].size();
    // MPI_Send(&sendSize, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    // for(int i=0 ; i<arr[0].size(); i++)
    // {
    //     MPI_Send(&(arr[0].at(i)), 1, PARTICLE, 1, i+1, MPI_COMM_WORLD);
    // }
    
    //**sends a bin(1msg method)
    // bin_t toSend;
    // for(int i=0 ; i<arr[0].size() ; i++)
    // {
    //     toSend.push_back(arr[0].at(i));
    // }
    
    // int sendSize = toSend.size();
    // MPI_Send(&toSend[0], sendSize, PARTICLE, 1, 0, MPI_COMM_WORLD);
    
    //**send row (doesnt work)
    // particle_t** toSend = (particle_t**)malloc(10 * 3 * sizeof(particle_t));
    // int sendSize = 0;
    // particle_t* tempBin = (particle_t*)malloc(3 * sizeof(particle_t));
    
    // for(int i=0 ; i<10 ; i++)
    // {
            
    //     for(int j=0 ; j<arr[i].size() ; j++)
    //     {
    //         // toSend[i][j] = arr[i].at(j);
    //         sendSize += 1;
    //         // std::cout << arr[i].at(j).x << " " << arr[i].at(j).y << std::endl;
            
    //         tempBin[j] = arr[i].at(j);
            
    //         // std::cout << tempBin[j].x << " " << tempBin[j].y << std::endl;
            
    //     }
        
    //     toSend[i] = tempBin;
    // }
    
    /**particle_t* tempBin = (particle_t*)malloc(3*sizeof(particle_t));
    particle_t* tempRow[10];
    particle_t* toSend = (particle_t*)malloc(10*3*sizeof(particle_t));
    
    for(int i=0 ; i<10 ; i++)
    {
        tempRow[i] = (particle_t*)malloc(1*sizeof(particle_t));
    }
    
    //1) convert the row, to a 2d array
    for(int i=0 ; i<10 ; i++)
    {
        for(int j=0 ; j<1 ; j++)
        {
            tempBin[j] = arr[i].at(j);
        }
        
        tempRow[i] = tempBin;
    }
    
    //2) print tempRow
    std::cout << "tempRow: ";
    for(int i=0 ; i<10 ; i++)
    {
        for(int j=0 ; j<1 ; j++)
        {
            std::cout << tempRow[i][j].x << " " << tempRow[i][j].y << " | ";
        }
        std::cout << std::endl; 
        
    }
    std::cout << std::endl;
    
    //3) Convert the 2d array to a 1d array
    toSend = convert2DArrayto1D(tempRow, 10, 1);
    
    //4)print toSend
    int sendCount = 0;
    std::cout << "toSend: ";
    for(int i=0 ; i<10 ; i++)
    {
        for(int j=0 ; j<1 ; j++)
        {
            std::cout << toSend[i*10+j].x << " " << toSend[i*10+j].y << " | ";
            sendCount++;
        }
        std::cout<<std::endl;
        
    }
    std::cout << std::endl;
    
    
    // // 5)send the converted array
    MPI_Send(&sendCount, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
    MPI_Send(&toSend[0], 10, PARTICLE, 1, 0, MPI_COMM_WORLD);**/
    
    int* sendGrid;
    sendGrid = convert2DIntArrayto1D(testGrid, 5, 5);
    
    std::cout << "Processor 0 Sent: " << std::endl;
      for(int i=0 ; i<5 ; i++)
      {
          for(int j=0 ; j<5 ; j++)
            std::cout << sendGrid[i*5 + j] << " | ";
        std::cout << std::endl;
      }
      std::cout << std::endl;
    
    MPI_Send(sendGrid, 25, MPI_INT, 1, 0, MPI_COMM_WORLD);
  }
  
  else if(rank == 1)
  {

    int* recvGrid = (int*)malloc(25*sizeof(int));
    MPI_Recv(recvGrid, 25, MPI_INT, 0, 0, MPI_COMM_WORLD, &Stat);
    std::cout << "Processor 1 Recieved: " << std::endl;
      for(int i=0 ; i<5 ; i++)
      {
          for(int j=0 ; j<5 ; j++)
            std::cout << recvGrid[i*5 + j] << " | ";
        std::cout << std::endl;
      }
      std::cout << std::endl;
      free(recvGrid);

    //6)recv the converted array
    /**int recvCount = 0;
    MPI_Recv(&recvCount, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &Stat);
    std::cout << "recvCount: " << recvCount << std::endl;
    
    recvRow = (particle_t*)malloc(recvCount*sizeof(particle_t));
    // MPI_Recv(&recvRow[0], recvCount, PARTICLE, 0, 0, MPI_COMM_WORLD, &Stat);
    
    //7)print recvRow
    std::cout << "recvRow: ";
    for(int i=0 ; i<10 ; i++)
    {
        std::cout << recvRow[i].x << " " << recvRow[i].y << " | ";
        
    }
    std::cout << std::endl;**/

    //**recv single particle
    // MPI_Recv(&recvPart, 1, PARTICLE, 0, 0, MPI_COMM_WORLD, &Stat);
    // std::cout << "Processor 1 Recieved: " << recvPart.x << " " << recvPart.y << std::endl;
    
    //**recieves a bin (loop method)    
    // int recvCount = 0;
    // MPI_Recv(&recvCount, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &Stat);
    
    // for(int i=0; i<recvCount ; i++)
    // {
    //     MPI_Recv(&recvPart, 1, PARTICLE, 0, i+1, MPI_COMM_WORLD, &Stat);
    //     recvBin.push_back(recvPart);
    // }
    
    //**recieves a bin(1 msg method)
    // particle_t *recvBinBuffer = (particle_t*)malloc(3 * sizeof(particle_t));
    // MPI_Recv(&recvBinBuffer[0], 1, PARTICLE, 0, 0, MPI_COMM_WORLD, &Stat);
    
    // for(int i=0 ; i<3 ; i++)
    // {
        // recvBin.push_back(recvBinBuffer[i]);
    // }
    
    //print bin
    // std::cout << "Processor 1 Recieved: ";
    // for(int j=0 ; j<1 ; j++)
    //     std::cout << recvBin[j].x << " " << recvBin[j].y << " | ";
    // std::cout << std::endl;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  
              
  MPI_Finalize();
}

particle_t* convert2DArrayto1D(particle_t** arr, int rows , int cols)
{
    particle_t* toReturn = (particle_t*)malloc(rows*cols*sizeof(particle_t));
    
    for(int i=0; i<rows ; i++)
    {
        for(int j=0 ; j<cols ; j++)
        {
            toReturn[i*rows + j] = arr[i][j]; 
        }
    }
    
    return toReturn;
}

int* convert2DIntArrayto1D(int** arr, int rows , int cols)
{
    int* toReturn = (int*)malloc(rows*cols*sizeof(int));
    
    for(int i=0; i<rows ; i++)
    {
        for(int j=0 ; j<cols ; j++)
        {
            toReturn[i*rows + j] = arr[i][j]; 
        }
    }
    
    return toReturn;
}