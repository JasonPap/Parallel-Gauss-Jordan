//#include <mpi.h>
#include <stdio.h>
#include "ColumnBlock.h"
#include "InitFunctions.h"

#define GROUPED     1
#define CYCLICALLY  0

int main(int argc, char *argv[])
{
    ///INITIALIZATION PHASE
    if ( argc != 3 )
    {
        printf("Wrong number of arguments.\n");
        printf("Usage: %s <array dimention> <grouped columns (0/1)>\n", argv[0]);
        return 1;
    }

    int array_dimention = atoi(argv[1]);
    int partition_mode = atoi(argv[2]);
    int** A = create2Darray(10);
    block* a = new block(1);

}
