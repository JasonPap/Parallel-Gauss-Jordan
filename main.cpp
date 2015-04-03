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
    block* proc_block = initialize(array_dimention, partition_mode);

    // test to see if the collumns were distributed corectly
    int myrank = MPI::COMM_WORLD.Get_rank();
    sleep(myrank*2);
    cout << "I am: "<<myrank<<endl;
    for ( auto it = proc_block->column.begin(); it != proc_block->column.end(); ++it )
        std::cout << "\t" << it->first;
    cout<<endl;

    // Compute the solution
    for ( int k = 0; k < array_dimention; k++ )
    {
        int max_val_id;
        if ( k is in my columns )
        {
            max_val_id = find_pivot(proc_block, k);
        }

        Ibroadcast(max_val_id); //one send the others receive
        proc_block->update_pivot(k, max_val_id);

        compute_values(proc_block, k);
    }


    // Finalize, print solution


    MPI_Barrier(MPI_COMM_WORLD);
}
