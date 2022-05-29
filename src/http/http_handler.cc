#include "http_handler.h"

#define READ_BUFFER_SIZE 1024

extern int yyparse();
extern void set_parsing_options(char *buf, size_t siz, ::Request_C *parsing_request);

namespace webserver
{

HttpHandler::HttpHandler() {}

HttpHandler::HttpHandler(GlobalConfig *_config) {
    config.reset(_config);
    processor.reset(new Processor(_config));
}

HttpHandler::~HttpHandler() {
}
   
void HttpHandler::init(int _sockfd, sockaddr_in &_client_addr) {
    sockfd = _sockfd;
    client_addr = _client_addr;
}

void HttpHandler::set_config(GlobalConfig *_config) {
    config.reset(_config);
}

void HttpHandler::set_processor() {
    processor.reset(new Processor(config.get()));
}

int HttpHandler::read_data() {
    char read_buffer[READ_BUFFER_SIZE];
    size_t index;
    while(true) {
        int ret = recv(sockfd, read_buffer, READ_BUFFER_SIZE - 1, 0);
        if (ret <= 0) {
            break;
        }
        size_t read_count = ret;
        if (stage == STAGE_BODY) {
            size_t left = body_length - cur_body_len;
            if (read_count < left) {
                append_data(read_buffer, read_count);
                cur_body_len += read_count;
            } else {
                // size_t buffer_count = buffer_list.get_count();
                LOG(INFO) << "message body receive completely";
                char buffer[body_length];
                buffer_list.get_all_data(buffer, cur_body_len);
                memcpy(buffer + cur_body_len, read_buffer, left);
                buffer_list.clear();
                buffer_list.append(buffer + left, ret - left);
                do_request(buffer, body_length, &request);
                stage = STAGE_HEADER;
            }
        } else {
            parse_read(read_buffer, read_count, &index);
            LOG(INFO) << "read_count: " << read_count << ", index: " << index;
            if (index < read_count) {
                // start line end
                LOG(INFO) << "receive a complete request header";
                size_t buffer_count = buffer_list.get_count();
                size_t total_count = buffer_count + index + 1;
                char buffer[total_count];
                buffer_list.get_all_data(buffer, buffer_count);
                memcpy(buffer + buffer_count, read_buffer, read_count);
                ret = parse_request_header(buffer, total_count);
                if (ret < 0) {
                    // parse error
                    process_error(ret);
                    return EINVALIDREQ;
                }
                if (has_request_body()) {
                    cur_body_len = 0;
                    body_length = request.get_body_length();
                    stage++;
                    LOG(INFO) << "request has message body, size is: " << body_length;
                } else {
                    do_request(NULL, 0, &request);
                }
                buffer_list.clear();
                size_t left_count = read_count - index - 1;
                if (left_count > 0) {
                    buffer_list.append(read_buffer + index + 1, left_count);
                }
            } else {
                append_data(read_buffer, read_count);
            }
        }
    }
    return 0;
}

void HttpHandler::append_data(char *data, size_t len) {
    buffer_list.append(data, len);
}

void HttpHandler::parse_read(char *data, size_t len, size_t *stop) {
    for (size_t i = 0; i < len; i++) {
        char ch = data[i];
        char expected = 0;
        switch (state) {
        case STATE_START:
        case STATE_CRLF:
            expected = '\r';
            break;
        case STATE_CR:
        case STATE_CRLFCR:
            expected = '\n';
            break;
        default:
            state = STATE_START;
            continue;
        }
        if (ch == expected) {
            state++;
        } else {
            state = STATE_START;
        }

        if (state == STATE_CRLFCRLF) {
            *stop = i;
            return;
        }
    }
    *stop = len;
}

int HttpHandler::parse_request_header(char *data, size_t len) {
    // parse the request header
    ::Request_C *request = (::Request_C *) malloc(sizeof(Request_C));
    memset(request, 0, sizeof(Request_C));
    request->header_count=0;
    request->header_capacity = 1;
    //TODO You will need to handle resizing this in parser.y
    request->headers = (::RequestHeader *) malloc(sizeof(::RequestHeader)*(request->header_capacity));
    set_parsing_options(data, len, request);
    if (yyparse() == SUCCESS) {
        // copy request to requset
        LOG(INFO) << "requet header is accepted";
        std::string method(request->http_method);
        std::string url(request->http_uri);
        std::string version(request->http_version);
        this->request.set_request_line(method, url, version);

        // copy request headers
        for (int i = 0; i < request->header_count; i++) {
            std::string name(request->headers[i].header_name);
            std::string value(request->headers[i].header_value);
            this->request.set_header(name, value);
        }

        return 0;
    } 
    LOG(ERROR) << "request parse failed";
    return EBADREQUEST;
}

void HttpHandler::do_request(char *body_data, size_t len, Request *request) {
    ClientInfo client_info;
    convert_address(client_addr, &client_info);
    RequestContext context(sockfd, *request, client_info);
    processor->do_request(context, body_data, len);
}

void HttpHandler::process_error(int error) {
    processor->process_error(sockfd, error);
}

bool HttpHandler::has_request_body() {
    return request.get_body_length() > 0;
}

void HttpHandler::close_conn(int epollfd) {
    if (sockfd != -1) {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, 0);
        close(sockfd);
        sockfd = -1;
    }
}

void HttpHandler::process() {
    read_data();
}
} // namespace webserver