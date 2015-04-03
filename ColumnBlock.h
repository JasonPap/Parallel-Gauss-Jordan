#ifndef _COLUMN_BLOCK_H_
#define _COLUMN_BLOCK_H_
#include <unordered_map>

class block
{
public:
    block(int column_size);
    ~block();
    bool add_column(int id, int* data);
    bool update_pivot(int k, int max_val_id);
    int find_pivot(int k);


    std::unordered_map<int, int*> column;   // key: column index, value: pointer to column

private:
    int* pivot_array;
    int column_size;
    int rank_id;
    int proc_num;
};
#endif // _COLUMN_BLOCK_H_
