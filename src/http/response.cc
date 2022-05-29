#include "response.h"
#include "errors.h"

namespace webserver {

Response::Response(std::string &_version, int _code_status, std::string &_desc) {
    status_line.version = _version;
    status_line.status_code = _code_status;
    status_line.reason = _desc;
}

int Response::validate() {
    // validate the method
    return OK;
}

int Response::append_data(std::string &_data) {
    data.append(_data);
    return 0;
}

int Response::append_header(std::string &_name, std::string &_value) {
    std::shared_ptr<ResponseHeader> header(new ResponseHeader(_name, _value));
    headers.push_back(header);
    return 0;
}

std::string Response::encode() {
    std::string request;
    
    // append request line
    request.append(status_line.version);    // append method
    request.append("\x20");                 // append space
    request.append(std::to_string(status_line.status_code));       // append url 
    request.append("\x20");                 // append space
    request.append(status_line.reason);   // append version
    request.append("\x0d\x0a");             // append crlf

    // append header
    for (size_t i = 0; i < headers.size(); i++) {
        request.append(headers[i]->field_name);  // append header name
        request.append(": ");                   // append colon
        request.append(headers[i]->field_value); // append filed value
        request.append("\x0d\x0a");             // append crlf
    }

    // append crlf
    request.append("\x0d\x0a"); 

    // append body
    request.append(data);

    return request;
}

}