#ifndef _PROCESSOR_
#define _PROCESSOR_

#include <sys/stat.h>
#include <fstream>
#include <glog/logging.h>
#include <cstdlib>
#include "request_context.h"
#include "global_config.h"
#include <sys/wait.h>
#include <sys/epoll.h>
#include <memory>

namespace webserver {

class Processor {
public:
    int do_request(webserver::RequestContext &request_context, char *data, size_t len);

    int process_error(int fd, int error);

    int process_get(int fd, Request &request, ClientInfo &client_info, char *data, size_t len);

    int process_post(int fd, Request &request, ClientInfo &client_info, char *data, size_t len);

    int process_head(int fd, Request &request, ClientInfo &client_info, char *data, size_t len);

    int process_cgi(int fd, Request &request, CgiUrl &cgi_info, ClientInfo &client_info, char *data, size_t len);

    Processor(GlobalConfig *_config);

private:
    void _set_cgi_env_variable(Request &request, CgiUrl &cgi_info, ClientInfo &client_info);

    std::shared_ptr<GlobalConfig> config;
}; // class RequestProcessor

}
#endif