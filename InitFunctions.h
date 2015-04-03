#ifndef _INIT_FUNCTIONS_H_
#define _INIT_FUNCTIONS_H_

#include "ColumnBlock.h"
float**   create2Darray(int dimention);
float*    create1Darray(int dimention);
block* initialize(int dimention, int partition_mode);
int initialize_MPI();

#endif //_INIT_FUNCTIONS_H_
