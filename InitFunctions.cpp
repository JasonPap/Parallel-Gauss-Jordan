#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include "InitFunctions.h"
#include "ColumnBlock.h"
#include "constants.h"

using namespace std;

float** create2Darray(int dimention)
{
    int i,j;
    //cout<<"generating array"<<endl;
    float** square_array = new float*[dimention];
    for ( i = 0; i < dimention; i++)
    {
        square_array[i] = new float[dimention];
        for ( j = 0; j < dimention; j++)
        {
            do
            {
                square_array[i][j] = rand()%100;
            }while (square_array[i][j] == 0);

        }
    }
    //cout<<"array generated"<<endl;

    ///
    /*square_array[0][0] = 1;
    square_array[1][0] = 2;
    square_array[2][0] = 0;
    square_array[3][0] = 1;
    square_array[0][1] = 2;
    square_array[1][1] = 2;
    square_array[2][1] = -3;
    square_array[3][1] = 21;
    square_array[0][2] = 4;
    square_array[1][2] = -3;
    square_array[2][2] = 5;
    square_array[3][2] = 1;
    square_array[0][3] = 6;
    square_array[1][3] = 14;
    square_array[2][3] = -6;
    square_array[3][3] = -5;*/
    ///

    return square_array;
}


float* create1Darray(int dimention)
{
    int i;
    float* vect = new float[dimention];
    for ( i = 0; i < dimention; i++)
    {
        vect[i] = rand()%100;
    }
    ///
    /*vect[0] = 0;
    vect[1] = -2;
    vect[2] = -7;
    vect[3] = 61;*/
    ///
    return vect;
}

block* initialize(int dimention, int partition_mode)
{
    int proc_num = initialize_MPI();
    srand(time(NULL));
    // Get the MPI process id
    int proc_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    MPI_Request* SendRequests; // will only be used by master
    float** tmpArrayA;

    block* proc_block = new block(dimention, partition_mode);    //arrays that will be used for the Gauss-Jordan Method
    if (proc_id == 0)   //if master process
    {
        tmpArrayA = create2Darray(dimention);
        float* B = create1Darray(dimention);
        proc_block->add_column(dimention, B);   //Master will always have the vector B

        // Async requests hadlers
        SendRequests = new MPI_Request[dimention];
        if ( partition_mode != distribution::ROUNDROBIN && partition_mode != distribution::GROUPED )
        {
            cerr<<"Invalid partition mode argument "<<partition_mode<<" , changing to RoundRobin."<<endl;
            partition_mode = distribution::ROUNDROBIN;
        }

        if ( partition_mode == distribution::ROUNDROBIN )
        {
            for ( int i = 0; i < dimention; i++)
            {
                MPI_Isend(tmpArrayA[i], dimention, MPI_FLOAT, i%proc_num,
                            i, MPI_COMM_WORLD, &(SendRequests[i]));
            }
        }
        else if( partition_mode = distribution::GROUPED )
        {
            for ( int i = 0; i < dimention; i++)
            {
                MPI_Isend(tmpArrayA[i], dimention, MPI_FLOAT, i/(dimention/proc_num),
                            i, MPI_COMM_WORLD, &(SendRequests[i]));
            }
        }

    }

    MPI_Request* ReceiveRequests = new MPI_Request[dimention/proc_num];
    // Every process receives it's columns
    for ( int i = 0, j = 0; i < dimention; i++)
    {
        if( partition_mode == distribution::ROUNDROBIN )
        {
            if ( i%proc_num == proc_id )
            {
                float* data = new float[dimention];
                MPI_Request request;
                MPI_Irecv((void*)data, dimention, MPI_FLOAT, 0, i, MPI_COMM_WORLD, &request);
                ReceiveRequests[j++] = request;
                proc_block->add_column(i, data);
            }
        }
        else if( partition_mode == distribution::GROUPED )
        {
            if ( i/(dimention/proc_num) == proc_id )
            {
                float* data = new float[dimention];
                MPI_Request request;
                MPI_Irecv((void*)data, dimention, MPI_FLOAT, 0, i, MPI_COMM_WORLD, &request);
                ReceiveRequests[j++] = request;
                proc_block->add_column(i, data);
            }
        }
    }

    //wait on receive
    MPI_Status* statuses = new MPI_Status[dimention/proc_num];
    int wait_retval = MPI_Waitall(dimention/proc_num, ReceiveRequests, statuses);
    if ( wait_retval != MPI_SUCCESS )
    {
        cerr<<"Init: Receive failed"<<endl;
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    delete[] statuses;
    delete[] ReceiveRequests;

    if ( proc_id == 0)
    {
        //wait on send
        MPI_Status* statuses = new MPI_Status[dimention];
        int wait_retval = MPI_Waitall(dimention, SendRequests, statuses);
        if ( wait_retval != MPI_SUCCESS )
        {
            cerr<<"Init: Send failed"<<endl;
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }

        for ( int j = 0; j<dimention; j++ )
        {
            delete[] tmpArrayA[j];
        }
        delete[] tmpArrayA;

        delete[] statuses;
        delete[] SendRequests;
    }
    return proc_block;
}

/*
 * Initializes the MPI framework
 * and return the number of MPI processes
 */
int initialize_MPI()
{
    int numprocs;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    return numprocs;
}
