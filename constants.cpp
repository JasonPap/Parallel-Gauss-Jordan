#include "constants.h"

bool column_is_local(int rank_id, int proc_num, int k,int mode)
{
    if ( mode == distribution::GROUPED )
    {
        if ( k/proc_num == rank_id )
            return true;
        else
            return false;
    }
    else if ( mode == distribution::ROUNDROBIN )
    {
        if ( k%proc_num == rank_id )
            return true;
        else
            return false;
    }
}

