//#include <mpi.h>
#include <stdio.h>
#include "ColumnBlock.h"
#include "InitFunctions.h"


int main(int argc, char *argv[])
{
    ///INITIALIZATION PHASE
    // Get command line arguments
    if ( argc != 3 )
    {
        printf("Wrong number of arguments.\n");
        printf("Usage: %s <array dimention> <grouped columns (0/1)>\n", argv[0]);
        return 1;
    }
    int array_dimention = atoi(argv[1]);
    int partition_mode = atoi(argv[2]);

    // Initialize the group of columns each process will get
    block* columns = initialize(array_dimention, partition_mode);

    // Compute the solution


    // Finalize, print solution

}
