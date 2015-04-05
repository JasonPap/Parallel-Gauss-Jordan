#include "Gauss-Jordan.h"


// search for the row with the bigger value,
// change the values with the current row and return
// the row id
int find_pivot(block* proc_block, int k)
{
    return proc_block->find_pivot(k);
}

int compute_values(block* proc_block, int k)
{
    for ( auto it = proc_block->column.begin(); it != proc_block->column.end(); ++it )
    {
        if ( it->first < k + 1 )    //only process columns from k+1 and on.
            continue;

        //proc_block->compute_column(it->first, k);
        /*float* column = it->second;
        float akj = column[1];*/

    }
    return 1;
}
