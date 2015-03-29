#include "InitFunctions.h"
#include <stdlib.h>
#include <time.h>

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
