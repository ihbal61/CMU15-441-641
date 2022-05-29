#ifndef _GLOBAL_CONFIG_
#define _GLOBAL_CONFIG_

#include <string>

struct GlobalConfig {
    std::string root;
    std::string cgi_root;
    std::string address;
    int port;
};

#endif