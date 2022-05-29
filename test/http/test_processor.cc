#include <gtest/gtest.h>
#include "processor.h"
#include <vector>

TEST(REQUEST_PROCESSOR, DO_REQUEST) {
    std::string http_version = "Http/1.1";
    std::string url = "/home/liaowangdong/index.html";
    std::string method = "GET";
    webserver::RequestLine request_line(method, url, http_version);
    std::vector<webserver::RequestHeader> headers;
    std::string data = "not data";
    // webserver::Request request(request_line, headers, data);
    // webserver::RequestContext request_context(0, request);
    // webserver::RequestProcessor processor;
    // processor.do_request(request_context);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}