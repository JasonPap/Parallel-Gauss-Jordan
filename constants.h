#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

enum distribution
{
    GROUPED = 1,
    ROUNDROBIN = 0
};

bool column_is_local(int rank_id, int proc_num, int k,int mode, int dimention);

#endif //_CONSTANTS_H_
