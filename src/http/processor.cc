#include "processor.h"
#include "errors.h"
#include "error_code.h"

namespace webserver {

Processor::Processor(GlobalConfig *_config) {
    config.reset(_config);
}

int Processor::do_request(webserver::RequestContext &request_context, char *data, size_t len) {
    Request &request = request_context.getRequest();
    int fd = request_context.getFd();
    ClientInfo &client_info = request_context.getClientInfo();
    int r = request.validate();
    if (r < 0) {
        return process_error(fd, r);
    }
    std::string url = request.get_url();
    CgiUrl cgi;
    if (parse_cgi_url(url, &cgi) == 0) {
        return process_cgi(fd, request, cgi, client_info, data, len);
    }
    std::string method = request.get_request_method();
    LOG(INFO) << "request type: " << method;
    if (method == "GET") {
        return process_get(fd, request, client_info, data, len);
    } else if (method == "POST") {
        return process_post(fd, request, client_info, data, len);
    } else {
        return process_head(fd, request, client_info, data, len);
    }
}

int Processor::process_cgi(int sockfd, Request &request, CgiUrl &cgi_info, ClientInfo &client_info,
                           char *data, size_t len) {
    LOG(INFO) << "cgi, data len: " << len;
    size_t raw_len = len;
    std::string script_rel_path = cgi_info.script_path;
    std::string cgi_root = config->cgi_root;
    std::string script_path = cgi_root.append(script_rel_path);
    if (file_exists(script_path) != 0) {
        LOG(ERROR) << "cgi script: { " << script_path << " }" << " not exists";
        int ret = process_error(sockfd, EFILENOTEXISTS);
        close(sockfd);
        return ret;
    }
    _set_cgi_env_variable(request, cgi_info, client_info);
    int pipe_fd[2];
    int ret = pipe(pipe_fd);
    if (ret < 0) {
        LOG(ERROR) << "pipe failed";
        ret = process_error(sockfd, EINTERNALERROR);
        close(sockfd);
        return ret;
    }
    ret = fork();
    if (ret < 0) {
        LOG(ERROR) << "fork failed";
        return -1;
    } else if (ret == 0) {
        // sub process
        // close write
        LOG(INFO) << "children process";
        close(STDIN_FILENO);
        int newfd = dup(pipe_fd[0]);
        LOG(INFO) << "newfd: " << newfd;
        close(STDOUT_FILENO);
        // make stdout redirect to sockfd
        dup(sockfd);
        close(pipe_fd[1]);
        execl(script_path.c_str(), script_path.c_str(), std::to_string(len).c_str(), NULL);
    }
    // close read
    LOG(INFO) << "start write data to fork process";
    close(pipe_fd[0]);
    int epollfd = epoll_create(5);
    epoll_event event;
    event.data.fd = pipe_fd[1];
    event.events = EPOLLOUT | EPOLLERR | EPOLLRDHUP;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
    epoll_event events[5];
    if (raw_len) {
        size_t offset = 0;
        while (true) {
            int ret = epoll_wait(epollfd, events, 5, -1);
            for (int i = 0; i < ret; i++) {
                if (events[i].data.fd == pipe_fd[1] && events[i].events & EPOLLOUT) {
                    ret = write(pipe_fd[1], data + offset, raw_len);
                    raw_len -= ret;
                    offset += ret;
                }
            }
            if (raw_len == 0) {
                LOG(INFO) << "no data to send, break while";
                break;
            }
        }
    }
    LOG(INFO) << "parent write all data, wait for children";
    // no data send to cgi ps, close the write
    close(pipe_fd[1]);
    int status;
    wait(&status);
    if (!WIFEXITED(status)) {
        // sub process failed
        return process_error(sockfd, EINTERNALERROR);
    }
    // close(sockfd);
    return 0;
}

int Processor::process_get(int fd, Request &request, ClientInfo &client_info, char *data, size_t len) {
    std::string url = request.get_url();
    std::string root = config->root;
    std::string file_path = root.append(url);
    if (file_exists(file_path) != 0) {
        LOG(ERROR) << "file: { " << file_path << " }" << " not exists";
        return process_error(fd, EFILENOTEXISTS);
    }
    std::string file_data;
    std::string line;
    std::ifstream file(file_path);
    if (file.is_open()) {
        while (getline(file, line)) {
            file_data.append(line);
        }
    }
    LOG(INFO) << "response to client 200";
    return response_200(fd, file_data);
}

int Processor::process_post(int fd, Request &request, ClientInfo &client_info, char *data, size_t len) {
    std::string url = request.get_url();
    std::string resp_data;
    return response_200(fd, resp_data);
}

int Processor::process_head(int fd, Request &request, ClientInfo &client_info, char *data, size_t len) {
    std::string url = request.get_url();
    std::string root = config->root;
    std::string file_path = root.append(url);
    if (file_exists(file_path) != 0) {
        LOG(ERROR) << "file: { " << file_path << " }" << " not exists";
        return process_error(fd, EFILENOTEXISTS);
    }
    std::string res_data;
    return response_200(fd, res_data);
}

int Processor::process_error(int fd, int error) {
    return response_with_error(fd, -error);
}

// todo: change to use macro
void Processor::_set_cgi_env_variable(Request &request, CgiUrl &cgi_info, ClientInfo &client_info) {
    std::string c_len_key("Content-Length");
    std::string content_length_str = request.get_header_value(c_len_key);
    setenv("CONTENT_LENGTH", content_length_str.c_str(), 1);

    std::string c_type_key = "Content-Type";
    std::string content_type = request.get_header_value(c_type_key);
    setenv("CONTENT_TYPE", content_type.c_str(), 1);

    setenv("GATEWAY INTERFACE", "CGI/1.1", 1);
    setenv("PATH_INFO", cgi_info.script_path.c_str(), 1);
    setenv("QUERY STRIN", cgi_info.query_string.c_str(), 1);

    std::string remote_addr = client_info.address.append(":").append(std::to_string(client_info.port));
    setenv("REMOTE ADDR", remote_addr.c_str(), 1);

    std::string request_method = request.get_request_method();
    setenv("REQUEST_METHOD", request_method.c_str(), 1);

    std::string request_url = request.get_url();
    setenv("REQUEST_URI", request_url.c_str(), 1);

    setenv("SCRIPT_NAME", cgi_info.script_path.c_str(), 1);

    setenv("SERVER_PORT", std::to_string(config->port).c_str(), 1);

    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);

    setenv("SERVER_SOFTWARE", "lwd_webserver/1.0", 1);

    std::string accept_key = "Accept";
    std::string http_accept = request.get_header_value(accept_key);
    setenv("HTTP_ACCEPT",http_accept.c_str(), 1);

    std::string referer_key = "Referer";
    std::string http_referer = request.get_header_value(referer_key);
    setenv("HTTP_REFERER", http_referer.c_str(), 1);

    std::string encoding_key = "Accept-Encoding";
    std::string http_accept_encoding = request.get_header_value(encoding_key);
    setenv("HTTP_ACCEPT_ENCODING", http_accept_encoding.c_str(), 1);

    std::string language_key = "Accept-Language";
    std::string http_accept_language = request.get_header_value(language_key);
    setenv("HTTP_ACCEPT_LANGUAGE", http_accept_language.c_str(), 1);

    std::string charset_key = "Accept-Charset";
    std::string http_accept_charset = request.get_header_value(charset_key);
    setenv("HTTP_ACCEPT_LANGUAGE", http_accept_charset.c_str(), 1);

    std::string host_key = "Host";
    std::string http_host = request.get_header_value(host_key);
    setenv("HTTP_HOST", http_host.c_str(), 1);

    std::string cookie_key = "Cookie";
    std::string http_cookie = request.get_header_value(cookie_key);
    setenv("HTTP_COOKIE", http_cookie.c_str(), 1);

    std::string user_agent_key = "User-Agent";
    std::string http_user_agent = request.get_header_value(user_agent_key);
    setenv("HTTP_USER_AGENT", http_user_agent.c_str(), 1);

    std::string connection_key = "Connection";
    std::string http_connection = request.get_header_value(connection_key);
    setenv("HTTP_CONNECTION", http_connection.c_str(), 1);
}

}