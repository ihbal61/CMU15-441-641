#include "util.h"

bool start_with(const std::string &str, const std::string &head) {
	return str.compare(0, head.size(), head) == 0;
} 

bool file_exists(std::string &name) {
    struct stat file_stat;
    if (stat(name.c_str(), &file_stat)) {
        return true;
    }
    return false;
}

int parse_cgi_url(std::string &url, CgiUrl *res) {
    std::string cgi_prefix("/cgi/");
    if (!start_with(url, cgi_prefix)) {
        return -1;
    }
    int index = url.find("?");
    if (index == -1) {
        res->script_path = url.substr(5, url.size());
    } else {
        res->script_path = url.substr(5, index);
        res->query_string = url.substr(index+1, url.size());
    }
    return 0;
}

int convert_address(sockaddr_in sock_addr, ClientInfo *client_info) {
    char client[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &sock_addr.sin_addr, client, INET_ADDRSTRLEN);
    client_info->address.append(client);
    client_info->port = sock_addr.sin_port;
    return 0;
}
