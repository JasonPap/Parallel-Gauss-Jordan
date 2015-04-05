#ifndef _COLUMN_BLOCK_H_
#define _COLUMN_BLOCK_H_
#include <unordered_map>

class block
{
public:
    block(int column_size, int partition_mode);
    ~block();
    bool add_column(int id, float* data);
    int find_pivot(int k);
    bool compute_values(int k);
    bool sync(int max_val_id, int k);
    bool local_column(int k);

    std::unordered_map<int, float*> column;   // key: column index, value: pointer to column

private:
    int get_root(int k);
    bool update_pivot(int k, int max_val_id);

    int* pivot_array;
    int column_size;
    int rank_id;
    int proc_num;
    int partition_mode;
    float* k_column;
};
#endif // _COLUMN_BLOCK_H_
