#ifndef _REQUEST_
#define _REQUEST_

#include <string>
#include <vector>
#include <memory>

namespace webserver {

struct RequestLine {
    std::string method;
    std::string url;
    std::string version;

    RequestLine() {}

    RequestLine(std::string &_method, std::string &_url, std::string &_version) {
        method = _method;
        url = _url;
        version = _version;
    }
};

struct RequestHeader {
    std::string field_name;
    std::string field_value;

    RequestHeader(std::string &_field_name, std::string &_field_value) {
        field_name = _field_name;
        field_value = _field_value;
    }
};


class Request {
public:

    Request();

    ~Request();

    void set_request_line(std::string &method, std::string &url, std::string &version);

    void set_header(std::string &name, std::string &value);

    void set_data(char *data);

    std::string& get_request_method();

    std::string& get_url();

    std::string& get_version();
    
    int validate();

    std::string encode();

    Request decode();
    
    std::string& get_header_value(std::string &name);

    char* get_data();
    
    size_t get_body_length();
private:
    RequestLine request_line;
    std::vector<std::shared_ptr<RequestHeader>> headers;
    char *data;
    size_t data_len;
}; // class Request

}
#endif