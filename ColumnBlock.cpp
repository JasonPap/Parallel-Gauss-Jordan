#include <unordered_map>
#include <iostream>
#include <cstdlib>
#include "ColumnBlock.h"

using namespace std;

block::~block()
{
    for ( auto it = column.begin(); it != column.end(); ++it )
        free(it->second);   //delete column data
}

bool block::add_column(int id, int* data)
{
    column[id] = data;
    return true;
}
