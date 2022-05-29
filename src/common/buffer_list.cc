#include "buffer_list.h"
#include <cstring>
#include <assert.h>

namespace webserver
{

// size not mean the buffer actual size, just info about the buffer size
BufferList::BufferList() {
    header = new Buffer();
    tail = header;
    count = 0;
    offset = 0;
    buffer_index.push_back(header);
}

BufferList::~BufferList() {
    for (auto ptr : buffer_index) {
        delete ptr;
    }
}

void BufferList::append(const char *data, size_t len) {
    size_t left = BUFFER_ITEM_SIZE - offset;
    size_t raw_len = len;
    if (left > len) {
        memcpy((void *)(&tail->data[offset]), data, len);
        offset += len;
    } else {
        // full the last buffer item
        size_t data_offset = 0;
        memcpy((void *)(&tail->data[offset]), data, left);
        data_offset += left;
        offset += left;
        len -= left;
        int buffer_count = (len + BUFFER_ITEM_SIZE - 1) / BUFFER_ITEM_SIZE;
        for (int i = 0; i < buffer_count; i++) {
            expand();
            if (i != buffer_count-1) {
                memcpy((void *)(&tail->data[offset]), data + data_offset, BUFFER_ITEM_SIZE);
                len -= BUFFER_ITEM_SIZE;
                data_offset += BUFFER_ITEM_SIZE;
                offset += BUFFER_ITEM_SIZE;
            } else {
                memcpy((void *)(&tail->data[offset]), data + data_offset, len);
                offset += len;
            }
        }
    }
    if (offset == BUFFER_ITEM_SIZE) {
        expand();
    }
    count += raw_len;
}

void BufferList::get(char *des, size_t *len) {
    *len = count;
    Buffer *buffer_ptr = header;
    int data_offset = 0;
    while (buffer_ptr != tail) {
        memcpy(des + data_offset, buffer_ptr->data, BUFFER_ITEM_SIZE);
        data_offset += BUFFER_ITEM_SIZE;
    }
    memcpy(des + data_offset, buffer_ptr->data, offset);
}

size_t BufferList::get_count() {
    return count;
}

void BufferList::expand() {
    assert(offset == BUFFER_ITEM_SIZE);
    tail->next = new Buffer();
    tail = tail->next;
    offset = 0;
    buffer_index.push_back(tail);
}

void BufferList::convert_index(size_t index, Buffer *target) {
    int buffer_idx = index / BUFFER_ITEM_SIZE;
    target = buffer_index[buffer_idx];
}

void BufferList::get_all_data(char *des, size_t len) {
    assert(count == len);
    if (count == 0) {
        des = NULL;
        return;
    }
    size_t buffer_item_count = buffer_index.size();
    size_t des_offset = 0;
    for (size_t i = 0; i < buffer_item_count - 1; i++) {
        memcpy(des + des_offset, buffer_index[i]->data, BUFFER_ITEM_SIZE);
        des_offset += BUFFER_ITEM_SIZE;
    }
    if (offset != 0) {
        memcpy(des + des_offset, buffer_index[buffer_item_count-1]->data, offset);
    }
}

void BufferList::clear() {
    while (header != tail) {
        Buffer *to_delete = header;
        header = header->next;
        delete to_delete;
    }
    buffer_index.clear();
    buffer_index.push_back(header);
    offset = 0;
    count = 0;
}

} // namespace webserver