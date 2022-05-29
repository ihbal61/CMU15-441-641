#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/epoll.h>

int main(int argc, char* argv[]) {
    std::string data;
    int ret;
    size_t data_len = atoi(argv[1]);
    std::cout << "data_len: " << data_len << std::endl;
    char buffer[1024];
    int epollfd = epoll_create(5);
    epoll_event event;
    event.data.fd = STDIN_FILENO;
    event.events = EPOLLIN | EPOLLERR | EPOLLRDHUP | EPOLLHUP;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
    epoll_event events[5];
    if (data_len) {
        while (true) {
            int ret = epoll_wait(epollfd, events, 5, -1);
            for (int i = 0; i < ret; i++) {
                int fd = events[i].data.fd;
                if ((fd == STDIN_FILENO) && (events[i].events & EPOLLIN)) {
                    ret = read(STDIN_FILENO, buffer, 1023);
                    data_len -= ret;
                    buffer[ret] = '\0';
                    std::cout << "new data: " << buffer << ", left: " << data_len << std::endl;
                    data.append(buffer, ret);
                } else if ((fd == STDIN_FILENO) && (events[i].events & (EPOLLERR | EPOLLRDHUP | EPOLLHUP))) {
                    break;
                } 
            }
            if (data_len == 0) {
                break;
            }
        }
    }
    std::string msg_data = "<html><header></header><body><h1>this is from cgi process</h1></body></html>";
    std::string status_line = "HTTP/1.1 200 OK\r\n";
    std::string cache_header = "Cache-Control: private, max-age=0, must-revalidate\r\n";
    std::string conn_header = "Connection: keep-alive\r\n";
    std::string content_header = "Content-Type: text/html; charset=utf-8\r\n";
    std::string kp_alive_timeout_header = "Keep-Alive: 20\r\n";
    std::string content_len = "Content-Length: " + std::to_string(msg_data.size()) + "\r\n";
    std::string crlf = "\r\n";
    std::cout << status_line << cache_header << conn_header << content_header << kp_alive_timeout_header << content_len << crlf << msg_data;
    
    // close(STDOUT_FILENO);
    // exit(0);
}