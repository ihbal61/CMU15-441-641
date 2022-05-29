#include "server.h"
#include <glog/logging.h>

namespace webserver {

HttpServer::HttpServer(GlobalConfig &_config) {
    config = _config;
    threadpool.reset(new ThreadPool(N_THREADS, MAX_FD));
    handlers.reset(new HttpHandler[MAX_FD]);
    _init_handler();
}

HttpServer::~HttpServer() {}

void HttpServer::_init_handler() {
    for (size_t i = 0; i < MAX_FD; i++) {
        handlers[i].set_config(&config);
        handlers[i].set_processor();
    }
}

int HttpServer::_setnoblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void HttpServer::_addfd(int epollfd, int fd, bool oneshot) {
    epoll_event *event = new epoll_event();
    event->data.fd = fd;
    event->events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if (oneshot) {
        event->events |= EPOLLONESHOT;
    }
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, event);
    if (ret < 0) {
        LOG(ERROR) << "add event to epoll failed";
    }
    _setnoblocking(fd);
}

void HttpServer::_removefd(int epollfd, int fd) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
}

void HttpServer::start() {
    std::string address = config.address;
    int port = config.port;
    sockaddr_in server_addr;
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &server_addr.sin_addr);

    // todo check the root dir exists, if not, create it

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("create socket failed: %d\n", errno);
        return;
    }

    int ret = bind(sockfd, (sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printf("bind failed: %d\n", errno);
        return;
    }

    ret = listen(sockfd, 10);
    if (ret < 0) {
        printf("listen failed: %d\n", errno);
        return;
    }
    LOG(INFO) << "server started, listen on: " << port;

    int epollfd = epoll_create(MAX_EVENT_COUNT);

    _addfd(epollfd, sockfd, false);
    // _setnoblocking(sockfd);
    epoll_event events[MAX_EVENT_COUNT];
    while (true) {
        ret = epoll_wait(epollfd, events, MAX_EVENT_COUNT, -1);
        if (ret < 0 && errno != EINTR) {
            LOG(ERROR) << "epoll failed";
            break;
        }
        for (int i = 0 ; i < ret; i++) {
            if ((events[i].data.fd == sockfd) && (events[i].events | EPOLLIN)) {
                // new connection
                sockaddr_in client_addr;
                socklen_t sock_len = sizeof(sockaddr_in);
                int connfd = accept(sockfd, (sockaddr *)&client_addr, &sock_len);
                if (connfd > MAX_FD) {
                    // too many connection, close(it)
                    continue;
                }
                LOG(INFO) << "new connection: " << connfd << ", port: " << client_addr.sin_port;
                handlers[connfd].init(connfd, client_addr);
                _addfd(epollfd, connfd, false);
            } else if (events[i].events & (EPOLLERR | EPOLLRDHUP | EPOLLHUP)) {
                // close conn
                int fd = events[i].data.fd;
                LOG(INFO) << "client " << fd << " close connection";
                handlers[fd].close_conn(epollfd);
            } else if (events[i].events | EPOLLIN) {
                // new data from client
                int connfd = events[i].data.fd;
                LOG(INFO) << "receive data from client: " << connfd;
                bool failed;
                int try_count = 0;
                do {
                    failed = threadpool->submit(&handlers[connfd]);
                    try_count++;
                    if (try_count >= 10)
                        break;
                } while (!failed);
            }
        }
    }
}

} // namespace webserve 
