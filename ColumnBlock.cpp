#include <unordered_map>
#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <string.h>
#include "ColumnBlock.h"
#include "constants.h"

using namespace std;

block::block(int column_size, int partition_mode)
{
    this->column_size = column_size;
    pivot_array = new int[column_size];
    this->partition_mode = partition_mode;

    rank_id = MPI::COMM_WORLD.Get_rank();
    proc_num = MPI::COMM_WORLD.Get_size();

    k_column = new float[column_size];

    // Initializing of pivot_array
    for (int i = 0; i<column_size; i++)
        pivot_array[i] = i;
}

block::~block()
{
    for ( auto it = column.begin(); it != column.end(); ++it )
        delete[] it->second;   //delete column data
    delete[] k_column;
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

/*bool block::compute_column(int column_num, int k)
{
    float* proc_column = column[column_num];
    float ajk = proc_column[pivot_array[k]/pivot_array[k]];
}*/

bool block::compute_values(int k)
{
    ///DEBUG
        if ( rank_id == 0)
        {   cout<<"k column id = "<<endl;
            for (int i = 0; i< column_size; i++)
            {
                cout<<k_column[i]<<", ";
            }cout<<endl;
        }
    ///END DEBUG

    float akk = k_column[pivot_array[k]];
    float* Ak = k_column;
    for ( auto it = column.begin(); it != column.end(); ++it )
    {
        if ( it->first < k + 1 )
            continue;

        float* Aj = it->second;

        Aj[pivot_array[k]] = Aj[pivot_array[k]]/akk;
        for ( int i = 0; i<column_size; i++)
        {
            if( i != k )
                Aj[pivot_array[i]] = Aj[pivot_array[i]] - Ak[pivot_array[i]]*Aj[pivot_array[k]];
        }

        ///DEBUG
        if ( rank_id == 0)
        {   cout<<"column id = "<<it->first<<endl;
            for (int i = 0; i< column_size; i++)
            {
                cout<<(column[column_size])[i]<<", ";
            }cout<<endl;
        }
        ///END DEBUG

    }
}

bool block::sync(int max_val_id, int k)
{
    int max_val_id_local = max_val_id;

    if ( local_column(k) )
    { //must send max_val_id and colum k
        //MPI_Request req;
        MPI_Bcast((void*)(&max_val_id), 1, MPI_INT, rank_id, MPI_COMM_WORLD);

        MPI_Bcast((void*)column[k], column_size, MPI_FLOAT, rank_id, MPI_COMM_WORLD);
        memcpy(k_column, column[k], column_size*sizeof(float));
        //cout<<"I am "<<rank_id<<", finished async broadcast."<<endl;
    }
    else
    { //receive
        int root = get_root(k);
        //cout<<"I am "<<rank_id<<", waiting broadcast from "<<root<<endl;
        MPI_Bcast((void*)(&max_val_id_local), 1, MPI_INT, root, MPI_COMM_WORLD);

        MPI_Bcast((void*)k_column, column_size, MPI_FLOAT, root, MPI_COMM_WORLD);
        //cout<<"I am "<<rank_id<<", finished sync broadcast."<<endl;
    }

    update_pivot(k, max_val_id_local);
    //cout<<"I am "<<rank_id<<", pivoted."<<endl;
    return true;
}

int block::get_root(int k)
{
    if ( partition_mode == distribution::GROUPED )
    {
        return k/proc_num;
    }
    else if( partition_mode == distribution::ROUNDROBIN )
    {
        return k%proc_num;
    }
}

bool block::local_column(int k)
{
    if ( column.count(k) != 0 )
        return true;
    else
        return false;
}
