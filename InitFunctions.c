#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include "InitFunctions.h"
#include "ColumnBlock.h"

int** create2Darray(int dimention)
{
    srand(time(NULL));
    int i,j;
    int** square_array = malloc(dimention * sizeof(int*));
    for ( i = 0; i < dimention; i++)
    {
        square_array[i] = malloc(dimention * sizeof(int));
        for ( j = 0; j < dimention; j++)
            square_array[i][j] = rand();
    }
    return square_array;
}


int* create1Darray(int dimention)
{
    srand(time(NULL));
    int i;
    int* vector = malloc(dimention * sizeof(int));
    for ( i = 0; i < dimention; i++)
    {
        vactor[i] = rand();
    }

    return vector;
}

block* initialize(int dimention)
{
    int proc_num = initialize_MPI();

    //Create datatypes (if needed)

    // Get the MPI process id
    int proc_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    MPI_Request* SendRequests; // will only be used by master

    block proc_block = new block(dimention);    //arrays that will be used for the Gauss-Jordan Method
    if (proc_id == 0)   //if master process
    {
        int** tmpArrayA = create2Darray(dimention);
        int* B = create1Darray(dimention);
        proc_block->add_column(dimention, B);   //Master will always have the vector B

        // Async requests hadlers
        SendRequests = malloc((dimention)*sizeof(MPI_Request));

        for ( int i = 0; i < dimention; i++)
        {
            MPI_Isend(tmpArrayA[i], dimention, MPI_INT, dimention%proc_num,
                        i, MPI_COMM_WORLD, &(SendRequests[i]));
            free(tmpArrayA[i]);
        }
        free(tmpArrayA);
    }

    MPI_Request* ReceiveRequests = malloc((dimention/proc_num) * sizeof(MPI_Request));
    // Every process receives it's columns
    for ( int i = 0; i < dimention; i++)
    {
        if ( dimention%proc_num == i )
        {
            int id;
            int* data = malloc(dimention * sizeof(int));
            MPI_Irecv((void*)data, dimention, MPI_INT, 0, i, MPI_COMM_WORLD, &(ReceiveRequests[i]))
            proc_block->add_column(id, data);
        }
    }

    //wait on receive
    MPI_Status* statuses = malloc(dimention/proc_num * sizeof(MPI_Status));
    int wait_retval = MPI_Waitall(dimention/proc_num, ReceiveRequests, statuses);
    if ( wait_retval != MPI_SUCCESS )
    {
        cerr<<"Init: Receive failed"<<endl;
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    free(statuses);
    free(ReceiveRequests);

    if ( proc_id == 0)
    {
        //wait on send
        MPI_Status* statuses = malloc(dimention * sizeof(MPI_Status));
        int wait_retval = MPI_Waitall(dimention, SendRequests, statuses);
        if ( wait_retval != MPI_SUCCESS )
        {
            cerr<<"Init: Send failed"<<endl;
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
        free(statuses);
        free(SendRequests);
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
