//#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include <mpi.h>
#include "ColumnBlock.h"
#include "InitFunctions.h"

using namespace std;

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

    // test to see if the collumns were distributed corectly
    int myrank = MPI::COMM_WORLD.Get_rank();
    sleep(myrank*2);
    cout << "I am: "<<myrank<<endl;
    for ( auto it = columns->column.begin(); it != columns->column.end(); ++it )
        std::cout << "\t" << it->first;
    cout<<endl;

    // Compute the solution



    // Finalize, print solution

    MPI_Barrier(MPI_COMM_WORLD);
}
