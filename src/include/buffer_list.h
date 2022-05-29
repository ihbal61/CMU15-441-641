#ifndef _BUFFER_LIST_
#define _BUFFER_LIST_

#include <sys/types.h>
#include <vector>
#define BUFFER_ITEM_SIZE 8

namespace webserver
{
struct Buffer {
    char data[BUFFER_ITEM_SIZE];
    Buffer *next;
};

class BufferList {
public:

    BufferList();

    void append(const char *data, size_t len);

    void get(char *des, size_t *len);

    void expand();

    size_t get_count();

    void convert_index(size_t index, Buffer *target);

    void get_all_data(char *des, size_t len);
    
    void clear();

    ~BufferList();
private:
    u_int64_t count; // byte
    Buffer *header;
    Buffer *tail;
    u_int16_t offset; // tail offset
    std::vector<Buffer*> buffer_index;
}; // class BufferList
} // namespace webserver
#endif