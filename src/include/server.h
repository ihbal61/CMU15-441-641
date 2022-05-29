#ifndef _SERVER_
#define _SERVER_

#include <cstdio>
#include <string>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <glog/logging.h>
#include <memory>
#include "global_config.h"
#include "threadpool.h"
#include "http_handler.h"

#define MAX_FD 1024
#define MAX_EVENT_COUNT 10000
#define N_THREADS 8

namespace webserver {
    
class HttpServer {
    friend class HttpHandler;
public:
    void start();

    HttpServer(GlobalConfig &_config);

    ~HttpServer();
private:
    void _addfd(int epollfd, int fd, bool oneshot);

    void _removefd(int epollfd, int fd);

    int _setnoblocking(int fd);

    void _init_handler();

    GlobalConfig config;
    std::unique_ptr<webserver::ThreadPool> threadpool;
    std::unique_ptr<webserver::HttpHandler[]> handlers;
}; // class HttpServer

}
#endif