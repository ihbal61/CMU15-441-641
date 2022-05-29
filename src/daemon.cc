#include <string>
#include "server.h"
#include <iostream>
#include <glog/logging.h>
#include "http_handler.h"
#include "request.h"
#include "global_config.h"

static GlobalConfig config;

void init_log(char *program) {
    FLAGS_logtostderr = true;
    FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(program);
}

void init_config() {
    config.root = "/home/liaowangdong/web/";
    config.cgi_root = "/home/liaowangdong/web/cgi/";
    config.address = "0.0.0.0";
    config.port = 50570;
}

int main(int argc, char *argv[])
{
    init_log(argv[0]);
    init_config();
    webserver::HttpServer* server = new webserver::HttpServer(config);
    server->start();
}