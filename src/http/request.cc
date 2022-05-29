#include <glog/logging.h>
#include "request.h"
#include "errors.h"

namespace webserver {

Request::Request() {}

Request::~Request() {}

void Request::set_request_line(std::string &method, std::string &url, std::string &version) {   
    request_line.method = method;
    request_line.url = url;
    request_line.version = version;
}

void Request::set_header(std::string &name, std::string &value) {
    std::shared_ptr<RequestHeader> header(new RequestHeader(name, value));
    headers.push_back(std::move(header));
}

void Request::set_data(char *_data) {
    data = _data;
}

std::string& Request::get_request_method() {
    return request_line.method;
}

std::string& Request::get_url() {
    return request_line.url;
}

std::string& Request::get_version() {
    return request_line.version;
}

int Request::validate() {
    // validate the method
    LOG(INFO) << "validate request header";
    std::string method = request_line.method;
    LOG(INFO) << "method: " << method;
    if (method != "GET" && method != "HEAD" && method != "POST") {
        LOG(ERROR) << "method must be GET, HEAD or POST";
        return EBADREQUEST;
    } 
    std::string version = request_line.version;
    if (version != "HTTP/1.1") {
        LOG(ERROR) << "http version must be HTTP/1.1";
        return EBADREQUEST;
    }
    return OK;
}

char* Request::get_data() {
    return data;
}

size_t Request::get_body_length() {
    for (size_t i = 0; i < headers.size(); i++) {
        if (headers[i]->field_name == "Content-Length") {
            return atoi(headers[i]->field_value.c_str());
        }
    }
    return 0;
}

std::string& Request::get_header_value(std::string &name) {
    std::string* value = new std::string();
    for (size_t i = 0; i < headers.size(); i++) {
        if (headers[i]->field_name == name) {
            *value = headers[i]->field_value;
        }
    }
    return *value;
}

std::string Request::encode() {
    std::string request;
    
    // append request line
    request.append(request_line.method);    // append method
    request.append("\x20");                 // append space
    request.append(request_line.url);       // append url 
    request.append("\x20");                 // append space
    request.append(request_line.version);   // append version
    request.append("\x0d\x0a");             // append crlf

    // append header
    for (size_t i = 0; i < headers.size(); i++) {
        request.append(headers[i]->field_name);  // append header name
        request.append(": ");                   // append colon
        request.append(headers[i]->field_value); // append filed value
        request.append("\x0d\x0a");             // append crlf
    }

    request.append("\x0d\x0a");     // append crlf;

    request.append(data);

    return request;
}

}