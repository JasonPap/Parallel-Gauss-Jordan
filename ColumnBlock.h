#include <unordered_map>

class block
{
public:
    block(int column_size){this->column_size = column_size;}
    ~block();
    bool add_column(int id, int* data);

    std::unordered_map<int, int*> column;

private:
    int column_size;
};

