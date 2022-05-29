#ifndef _ERROR_CODE_
#define _ERROR_CODE_

#include <map>
#include "response.h"
#include <iostream>
#include <sys/socket.h>

namespace webserver {

#define HTTP_VERSION "HTTP/1.1"

int send_data(int fd, const char *data, size_t len) {
    int ret;
    size_t offset = 0;
    while (len) {
        ret = send(fd, data + offset, len, 0);
        if (ret < 0) {
            if (errno == EAGAIN) {
                continue;
            }
            return -1;
        }
        len -= ret;
    }
    return 0;
}

int add_response_header(Response &response) {
    std::string cache_header_name = "Cache-Control";
    std::string cache_header_value = "private, max-age=0, must-revalidate";

    std::string conn_header_name = "Connection";
    std::string conn_header_value = "keep-alive";

    std::string content_header_name = "Content-Type";
    std::string content_header_value = "text/html; charset=utf-8";
    
    std::string kp_alive_timeout_header_name = "Keep-Alive";
    std::string kp_alive_timeout_value = "20";

    response.append_header(cache_header_name, cache_header_value);
    response.append_header(conn_header_name, conn_header_value);
    response.append_header(content_header_name, content_header_value);
    response.append_header(kp_alive_timeout_header_name, kp_alive_timeout_value);
    return 0;
}

std::map<int, std::string> code_desc_map = {
    {100, "Continue"},
    {101, "Switching Protocols"},
    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {203, "Non-Authoritative Information"},
    {204, "No Content"},
    {205, "Reset Content"},
    {206, "Partial Content"},
    {300, "Multiple Choices"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {303, "See Other"},
    {304, "Not Modified"},
    {305, "Use Proxy"},
    {307, "Temporary Redirect"},
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {402, "Payment Required"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {406, "Not Acceptabl"},
    {407, "Proxy Authentication Required"},
    {408, "Request Time-out"},
    {409, "Conflict"},
    {410, "Gone"},
    {411, "Length Required"},
    {412, "Precondition Failed"},
    {413, "Request Entity Too Large"},
    {414, "Request-URI Too Large"},
    {415, "Unsupported Media Type"},
    {416, "Requested range not satisfiable"},
    {417, "Expectation Failed"},
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Time-out"},
    {505, "HTTP Version not supported"}
};

int response_200(int fd, std::string &data) {
    std::string code_desc = code_desc_map.at(200);
    std::string http_version(HTTP_VERSION);
    webserver::Response response(http_version, 200, code_desc);
    add_response_header(response);
    if (data.size() > 0) {
        std::string name = "Content-Length";
        std::string size_str = std::to_string(data.size());
        response.append_header(name, size_str);
    }
    response.append_data(data);
    std::string respone_data = response.encode();
    send_data(fd, respone_data.c_str(), respone_data.size());
    return OK;
}

int response_with_error(int fd, int error_code) {
    std::string code_desc = code_desc_map.at(error_code);
    std::string http_version(HTTP_VERSION);
    webserver::Response response(http_version, error_code, code_desc);
    add_response_header(response);
    std::string respone_data = response.encode();
    send_data(fd, respone_data.c_str(), respone_data.size());
    return OK;
}
} // namespace webserve 
#endif