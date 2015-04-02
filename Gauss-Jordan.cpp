#include "Gauss-Jordan.h"


// search for the row with the bigger value,
// change the values with the current row and return
// the row id
int find_pivot(block* proc_block, int k)
{
    return proc_block->find_pivot(k);
}

