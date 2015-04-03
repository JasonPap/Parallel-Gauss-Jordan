#include <unordered_map>
#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include "ColumnBlock.h"

using namespace std;

block::block(int column_size)
{
    this->column_size = column_size;
    pivot_array = new int[column_size];

    rank_id = MPI::COMM_WORLD.Get_rank();
    proc_num = MPI::COMM_WORLD.Get_size();

    // Initializing of pivot_array
    for (int i = 0; i<column_size; i++)
        pivot_array[i] = i;
}

block::~block()
{
    for ( auto it = column.begin(); it != column.end(); ++it )
        free(it->second);   //delete column data
}

bool block::add_column(int id, float* data)
{
    column[id] = data;
    return true;
}


// find the max of column k
// then return the id of the row that have the max elem.
int block::find_pivot(int k)
{
    float* current_column = column[k];
    float max_val = current_column[pivot_array[k]];
    int max_val_id = k;
    for ( int i = k + 1; k < column_size; k++ )
    {
        if ( current_column[pivot_array[i]] > max_val )
        {
            max_val = current_column[pivot_array[i]];
            max_val_id = i;
        }
    }

    //update_pivot(k, max_val_id);
    return max_val_id;
}

//swap k-th element of pivot array with max_val_id-th
bool block::update_pivot(int k, int max_val_id)
{

    int tmp = pivot_array[k];
    pivot_array[k] = pivot_array[max_val_id];
    pivot_array[max_val_id] = tmp;
    return true;
}

bool block::compute_column(int column_num, int k)
{
    float* proc_column = column[column_num];
    float ajk = proc_column[pivot_array[k]/pivot_array[k]];
}
