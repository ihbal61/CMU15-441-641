#ifndef _RESPONSE_
#define _RESPONSE_

#include <string>
#include <vector>
#include <memory>

namespace webserver {

struct StatusLine {
    std::string version;
    int status_code;
    std::string reason;

    StatusLine() {}

    StatusLine(std::string &_version, int _status_code, std::string &_reason) {
        version = _version;
        status_code = _status_code;
        reason = _reason;
    }
}; 

struct ResponseHeader {
    std::string field_name;
    std::string field_value;

    ResponseHeader() {}
    
    ResponseHeader(std::string &_field_name, std::string &_field_value) {
        field_name = _field_name;
        field_value = _field_value;
    }
};

class Response {
public:

    Response(std::string &_version, int _code_status, std::string &_desc);

    int validate();

    int append_data(std::string &_data);

    int append_header(std::string &_name, std::string &_value);

    std::string encode();

    Response decode();

private:
    StatusLine status_line;
    std::vector<std::shared_ptr<ResponseHeader>> headers;
    std::string data;
}; // class Response

}
#endif