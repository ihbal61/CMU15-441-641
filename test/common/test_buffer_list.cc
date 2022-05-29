#include <string>
#include <gtest/gtest.h>
#include "buffer_list.h"

TEST(TEST_BUFFER_LIST, APPEND_DATA) {
    std::string str = "1234567";
    std::string expected_str = "";
    int count = 5;
    webserver::BufferList buffer_list;
    for (int i = 0; i < count; i++) {
        buffer_list.append(str.c_str(), str.size());
        expected_str.append(str);
    }
    u_int32_t buffer_size =  buffer_list.get_count();
    char actual_str[buffer_size];
    buffer_list.get_all_data(actual_str, buffer_size);
    const char *expected_char_arr = expected_str.c_str();
    for (int i = 0; i < buffer_size; i++) {
        ASSERT_TRUE(*(expected_char_arr + i) == actual_str[i]);
    }
}

TEST(TEST_BUFFER_LIST, CLEAR_DATA) {
    std::string str = "1234567";
    int count = 5;
    webserver::BufferList buffer_list;
    for (int i = 0; i < count; i++) {
        buffer_list.append(str.c_str(), str.size());
    }
    u_int32_t buffer_size =  buffer_list.get_count();
    char actual_str[buffer_size + 1];
    buffer_list.get_all_data(actual_str, buffer_size);
    buffer_list.clear();
    u_int32_t cleared_count = buffer_list.get_count();
    char cleared_data[cleared_count + 1];
    buffer_list.get_all_data(cleared_data, cleared_count);
    ASSERT_TRUE(cleared_count == 0);
}

TEST(TEST_BUFFER_LIST, FULL_BUFFER_DATA) {
    std::string request_line = "GET index.html HTTP/1.1\r\n";
    std::string host_header = "Host: www.baidu.com\r\n";
    std::string crlf = "\r\n";
    std::string expected_string;
    expected_string.append(request_line).append(host_header).append(crlf);
    webserver::BufferList buffer_list;
    buffer_list.append(request_line.c_str(), request_line.size());
    buffer_list.append(host_header.c_str(), host_header.size());
    buffer_list.append(crlf.c_str(), crlf.size());
    size_t buffer_size =  buffer_list.get_count();
    char actual_str[buffer_size + 1];
    buffer_list.get_all_data(actual_str, buffer_size);
    actual_str[buffer_size] = '\0';
    ASSERT_TRUE(buffer_size == expected_string.size());
    std::string actual_string(actual_str);
    ASSERT_TRUE(actual_string == expected_string);
}