#ifndef _UTIL_
#define _UTIL_

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

struct CgiUrl {
    std::string script_path;
    std::string query_string;
};

struct ClientInfo {
    std::string address;
    int port;
};

bool start_with(const std::string &str, const std::string &head);

bool file_exists(std::string &name);

int parse_cgi_url(std::string &url, CgiUrl *res);

int convert_address(sockaddr_in sock_addr, ClientInfo *client_info);

#endif