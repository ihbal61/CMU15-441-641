#ifndef _HTTP_HANDLER_
#define _HTTP_HANDLER_

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <glog/logging.h>
#include "buffer_list.h"
#include "request.h"
#include <cstring>
#include "errors.h"
#include "parse.h"
#include "processor.h"
#include "request_context.h"
#include "global_config.h"

namespace webserver
{
enum {
    STATE_START = 0, STATE_CR, STATE_CRLF, STATE_CRLFCR, STATE_CRLFCRLF
};

enum {
    STAGE_HEADER = 0, STAGE_BODY
};

class HttpHandler {
public:
    HttpHandler();

    HttpHandler(GlobalConfig *_config);

    void init(int _sockfd, sockaddr_in &client_addr);

    void set_config(GlobalConfig *_config);

    void set_processor();
    
    int read_data();

    void append_data(char *data, size_t len);

    void parse_read(char *data, size_t len, size_t *stop);

    int parse_request_header(char *data, size_t len);

    void do_request(char *body_data, size_t len, Request *request);

    void process_error(int error);

    bool has_request_body();

    void close_conn(int epollfd);

    void process();

    ~HttpHandler();
    
private:
    int sockfd;
    sockaddr_in client_addr;
    int stage;
    BufferList buffer_list;
    int state; // 
    size_t cur_offset;
    size_t body_length;
    size_t cur_body_len;
    Request request;
    std::shared_ptr<GlobalConfig> config;
    std::unique_ptr<Processor> processor;
}; // class RequestHandler

} // namespace webserver
#endif