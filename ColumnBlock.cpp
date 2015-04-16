#include <map>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
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
    for ( map<int,float*>::iterator it = column.begin(); it != column.end(); ++it )
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
    int i;
    for ( i = k + 1; i < column_size; i++ )
    {
        if ( current_column[pivot_array[i]] > max_val )
        {
            max_val = current_column[pivot_array[i]];
            max_val_id = i;
        }
    }

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

bool block::compute_values(int k)
{
    float akk = k_column[pivot_array[k]];
    float* Ak = k_column;
    for ( map<int,float*>::iterator it = column.begin(); it != column.end(); ++it )
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
    }
}

bool block::sync(int max_val_id, int k)
{
    int max_val_id_local = max_val_id;

    if ( local_column(k) )
    { //must send max_val_id and colum k
        MPI_Bcast((void*)(&max_val_id), 1, MPI_INT, rank_id, MPI_COMM_WORLD);

        MPI_Bcast((void*)column[k], column_size, MPI_FLOAT, rank_id, MPI_COMM_WORLD);
        memcpy(k_column, column[k], column_size*sizeof(float));
    }
    else
    { //receive
        int root = get_root(k);
        MPI_Bcast((void*)(&max_val_id_local), 1, MPI_INT, root, MPI_COMM_WORLD);

        MPI_Bcast((void*)k_column, column_size, MPI_FLOAT, root, MPI_COMM_WORLD);
    }

    update_pivot(k, max_val_id_local);
    return true;
}

bool block::sync2(int max_val_id, int k)
{
    int max_val_id_local = max_val_id;

    float* tmpbuffer = new float[column_size+1];

    if ( local_column(k) )
    { //must send max_val_id and colum k
        MPI_Request req;

        memcpy(tmpbuffer + 1, column[k], column_size*sizeof(float));
        memcpy(tmpbuffer, &max_val_id, sizeof(int));

        for ( int j = 1; j < proc_num; j++)
        {
            for ( int i = k + 1; i < column_size; i++ )
            {
                int dest = get_root(i);
                if ( dest == rank_id)
                    continue;


                if( dest == j)
                {
                    MPI_Isend((void*)(tmpbuffer), column_size + 1, MPI_FLOAT, dest,
                            rank_id, MPI_COMM_WORLD, &req);
                    break;
                }
            }
        }
        //always send to master because he has the b array
        if ( rank_id != 0 )
        {
            MPI_Isend((void*)(tmpbuffer), column_size + 1, MPI_FLOAT, 0,
                            rank_id, MPI_COMM_WORLD, &req);
        }
        memcpy(k_column, column[k], column_size*sizeof(float));
    }
    else
    { //receive
        //if I have a column with id > k
        bool must_receive = false;
        for ( map<int,float*>::iterator it = column.begin(); it != column.end(); ++it )
        {
            if ( it->first > k)
                must_receive = true;
        }

        if(must_receive)
        {
            int root = get_root(k);
            MPI_Request req;
            MPI_Status stat;

            MPI_Irecv((void*)tmpbuffer, column_size + 1, MPI_FLOAT, root,
                        root, MPI_COMM_WORLD, &req);
            MPI_Wait(&req, &stat);
            memcpy(&max_val_id_local, tmpbuffer, sizeof(int));
            memcpy(k_column, tmpbuffer+1, column_size*sizeof(float));
        }
    }

    update_pivot(k, max_val_id_local);
    delete []tmpbuffer;
    return true;
}

int block::get_root(int k)
{
    if ( partition_mode == distribution::GROUPED )
    {
        return k/(column_size/proc_num);
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

void block::print_pivot()
{
    for (int i = 0; i<column_size; i++)
        cout<<"piv["<<i<<"] = "<<pivot_array[i]<<endl;
}

void block::print_solution()
{
    if ( rank_id != 0 )
        return;

    cout<<"SOLUTION:"<<endl;
    for ( int i = 0; i < column_size; i++ )
    {
        cout<<"x"<<pivot_array[i]<< " = "<<(column[column_size])[i]<<endl;
    }
    return;
}
