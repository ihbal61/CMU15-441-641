#include <gtest/gtest.h>
#include <cstring>
#include <iostream>
#include "parse.h"

extern void set_parsing_options(char *buf, size_t size, Request_C *request);
extern int yyparse();

TEST(TEST_HTTP_PARSER, TEST_REQUEST_NO_HEADER) {
    std::string request_line = "GET /~prs/15-441-F15/1 HTTP/1.1\r\n\r\n";
    char data[request_line.size()];
    request_line.copy(data, request_line.size(), 0);
    std::cout << "data: " << data << std::endl;
    Request_C *request = (Request_C *) malloc(sizeof(Request_C));
    request->headers = (RequestHeader *) malloc(sizeof(RequestHeader)*1);
    request->header_count = 0;
    request->header_capacity = 1;
    set_parsing_options(data, request_line.size(), request);
    ASSERT_TRUE(yyparse() == 0);
    ASSERT_TRUE(strcmp(request->http_version,"HTTP/1.1") == 0);
    ASSERT_TRUE(strcmp(request->http_uri, "/~prs/15-441-F15/1") == 0);
    ASSERT_TRUE(strcmp(request->http_method,"GET") == 0);
    ASSERT_TRUE(request->header_count == 0);
}

TEST(TEST_HTTP_PARSER, TEST_REQUEST_ONE_HEADER) {
    std::string request_line = "GET /~prs/15-441-F15/1 HTTP/1.1\r\n";
    std::string header = "Host: www.baidu.com\r\n";
    std::string crlf = "\r\n";
    std::string data;
    data.append(request_line);
    data.append(header);
    data.append(crlf);
    char c_data[data.size()];
    data.copy(c_data, data.size(), 0);
    Request_C *request = (Request_C *) malloc(sizeof(Request_C));
    request->headers = (RequestHeader *) malloc(sizeof(RequestHeader)*1);
    request->header_count = 0;
    request->header_capacity = 1;
    set_parsing_options(c_data, data.size(), request);
    ASSERT_TRUE(yyparse() == 0);
    ASSERT_TRUE(strcmp(request->http_version,"HTTP/1.1") == 0);
    ASSERT_TRUE(strcmp(request->http_uri, "/~prs/15-441-F15/1") == 0);
    ASSERT_TRUE(strcmp(request->http_method,"GET") == 0);
    ASSERT_TRUE(request->header_count == 1);

    // assert request header
    ASSERT_TRUE(strcmp(request->headers[0].header_name,"Host") == 0);
    ASSERT_TRUE(strcmp(request->headers[0].header_value, "www.baidu.com") == 0);
}

TEST(TEST_HTTP_PARSER, TEST_REQUEST_MULTI_HEADER) {
    std::string request_line = "GET /~prs/15-441-F15/1 HTTP/1.1\r\n";
    std::string host_header = "Host: www.baidu.com\r\n";
    std::string conn_header = "Connection: keep-alive\r\n";
    std::string accept_header = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n";
    std::string agent_header = "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36\r\n";
    std::string encode_header = "Accept-Encoding: gzip, deflate, sdch\r\n";
    std::string language_header = "Accept-Language: en-US,en;q=0.8\r\n";
    std::string crlf = "\r\n";
    std::string data;
    data.append(request_line);
    data.append(host_header);
    data.append(conn_header);
    data.append(accept_header);
    data.append(agent_header);
    data.append(encode_header);
    data.append(language_header);
    data.append(crlf);
    char c_data[data.size()];
    data.copy(c_data, data.size(), 0);
    Request_C *request = (Request_C *) malloc(sizeof(Request_C));
    request->headers = (RequestHeader *) malloc(sizeof(RequestHeader)*1);
    request->header_count = 0;
    request->header_capacity = 1;
    set_parsing_options(c_data, data.size(), request);
    ASSERT_TRUE(yyparse() == 0);
    ASSERT_TRUE(strcmp(request->http_version,"HTTP/1.1") == 0);
    ASSERT_TRUE(strcmp(request->http_uri, "/~prs/15-441-F15/1") == 0);
    ASSERT_TRUE(strcmp(request->http_method,"GET") == 0);
    ASSERT_TRUE(request->header_count == 6);

    // assert request header
    ASSERT_TRUE(strcmp(request->headers[0].header_name,"Host") == 0);
    ASSERT_TRUE(strcmp(request->headers[0].header_value, "www.baidu.com") == 0);

    ASSERT_TRUE(strcmp(request->headers[1].header_name,"Connection") == 0);
    ASSERT_TRUE(strcmp(request->headers[1].header_value, "keep-alive") == 0);

    ASSERT_TRUE(strcmp(request->headers[2].header_name,"Accept") == 0);
    ASSERT_TRUE(strcmp(request->headers[2].header_value, "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8") == 0);

    ASSERT_TRUE(strcmp(request->headers[3].header_name,"User-Agent") == 0);
    ASSERT_TRUE(strcmp(request->headers[3].header_value, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36") == 0);

    ASSERT_TRUE(strcmp(request->headers[4].header_name,"Accept-Encoding") == 0);
    ASSERT_TRUE(strcmp(request->headers[4].header_value, "gzip, deflate, sdch") == 0);

    ASSERT_TRUE(strcmp(request->headers[5].header_name,"Accept-Language") == 0);
    ASSERT_TRUE(strcmp(request->headers[5].header_value, "en-US,en;q=0.8") == 0);
}