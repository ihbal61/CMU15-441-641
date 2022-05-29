
.DEFAULT_GOAL := server

OBJ_DIR = obj

BUFFER_LIST_SRC = src/common/buffer_list.cc
BUFFER_LIST_OBJ = src/common/buffer_list.o

CONTEXT_SRC = src/common/context.cc
CONTEXT_OBJ = src/common/context.o

REQUEST_CONTEXT_SRC = src/common/request_context.cc
REQUEST_CONTEXT_OBJ = src/common/request_context.o

THREADPOOL_SRC = src/common/threadpool.cc
THREADPOOL_OBJ = src/common/threadpool.o

UTIL_SRC = src/common/util.cc
UTIL_OBJ = src/common/util.o

HTTP_HANDLER_SRC = src/http/http_handler.cc
HTTP_HANDLER_OBJ = src/http/http_handler.o

PROCESSOR_SRC = src/http/processor.cc
PROCESSOR_OBJ = src/http/processor.o

REQUEST_SRC = src/http/request.cc
REQUEST_OBJ = src/http/request.o

RESPONSE_SRC = src/http/response.cc
RESPONSE_OBJ = src/http/response.o

SERVER_SRC = src/http/server.cc
SERVER_OBJ = src/http/server.o

LEX_YY_SRC = src/parser/lex.yy.c
LEX_YY_OBJ = src/parser/lex.yy.o

Y_TAB_SRC = src/parser/y.tab.c
Y_TAB_OBJ = src/parser/y.tab.o

DAEMON_SRC = src/daemon.cc
DAEMON_OBJ = src/daemon.o

COMMON_SRCS = $(BUFFER_LIST_SRC) $(CONTEXT_SRC) $(REQUEST_CONTEXT_SRC) $(THREADPOOL_SRC) $(UTIL_SRC)
COMMON_OBJS = $(BUFFER_LIST_OBJ) $(CONTEXT_OBJ) $(REQUEST_CONTEXT_OBJ) $(THREADPOOL_OBJ) $(UTIL_OBJ)

HTTP_SRCS = $(HTTP_HANDLER_SRC) $(PROCESSOR_SRC) $(REQUEST_SRC) $(RESPONSE_SRC) $(SERVER_SRC)
HTTP_OBJS = $(HTTP_HANDLER_OBJ) $(PROCESSOR_OBJ) $(REQUEST_OBJ) $(RESPONSE_OBJ) $(SERVER_OBJ)

PARSER_SRCS = $(LEX_YY_SRC) $(Y_TAB_SRC)
PARSER_OBJS = $(LEX_YY_OBJ) $(Y_TAB_OBJ)

CXX_FILES = $(COMMON_SRCS) $(HTTP_SRCS) $(DAEMON_SRC)
OBJS = $(COMMON_OBJS) $(HTTP_OBJS) $(DAEMON_OBJ)

INCLUDES = -I src/include/
INCLUDES += -I src/conf/

LIBPATH = -L./lib/

CXX_FLAGS = -std=c++11 -Wall -g

CC = gcc

CXX = g++

LIBS = -lpthread -lglog

server: $(OBJS) $(PARSER_OBJS)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -o $@ $(OBJS) $(PARSER_OBJS) $(LIBS)

$(PARSER_OBJS): %.o : %.c
	$(CXX) $(CXX_FLAGS) -c $< -o $@ $(INCLUDES)

$(OBJS): %.o : %.cc
	$(CXX) $(CXX_FLAGS) -c $< -o $@ $(INCLUDES)

TEST_BUFFER_LIST_SRC = test/common/test_buffer_list.cc
TEST_BUFFER_LIST_OBJ = test/common/test_buffer_list.o

TEST_HTTP_PARSE_SRC = test/parser/test_http_parse.cc
TEST_HTTP_PARSE_OBJ = test/parser/test_http_parse.o

test_buffer_list: $(TEST_BUFFER_LIST_OBJ) $(BUFFER_LIST_OBJ)
	$(CXX) -o $@ $(BUFFER_LIST_OBJ) $(TEST_BUFFER_LIST_OBJ) $(LIBPATH) -lgtest -lgtest_main -lpthread

$(TEST_BUFFER_LIST_OBJ): $(TEST_BUFFER_LIST_SRC)
	$(CXX) -c $< -o $@ $(INCLUDES) $(LIBPATH) -lgtest -lgtest_main

test_http_parse: $(TEST_HTTP_PARSE_OBJ) $(PARSER_OBJS)
	$(CXX) -o test_http_parse $(TEST_HTTP_PARSE_OBJ) $(PARSER_OBJS) $(LIBPATH) -lgtest -lgtest_main -lpthread

$(TEST_HTTP_PARSE_OBJ): $(TEST_HTTP_PARSE_SRC)
	$(CXX) -c $< -o $@ $(INCLUDES) $(LIBPATH) -lgtest -lgtest_main

.PHONY:clean
clean:
	find ./ -name *.o | xargs rm -f

