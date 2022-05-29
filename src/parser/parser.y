%{
#include <stdio.h>
#include "parse.h"

extern int yylex();

void yyerror(const char *str)
{
    fprintf(stderr, "error: %s\n", str);
}

int yywrap()
{
    return 1;
}

// int main()
// {
//     yyparse();
// }

void set_parsing_options(char *buf, size_t siz, Request_C *parsing_request);

/* Pointer to the buffer that contains input */
char *parsing_buf;

/* Current position in the buffer */
int parsing_offset;

/* Buffer size */
size_t parsing_buf_size;

/* Request Struct */
Request_C *parsing_request;

void expand_request_header();
%}

%union {
	char str[8192];
	int i;
}

%start request

%token t_crlf
%token t_backslash
%token t_digit
%token t_number
%token t_dot
%token t_token_char
%token t_lws
%token t_colon
%token t_separators
%token t_sp
%token t_ws
%token t_ctl

/* Type of value returned for these tokens */
%type<str> t_crlf
%type<i> t_backslash
%type<i> t_digit
%type<i> t_number
%type<i> t_dot
%type<i> t_token_char
%type<str> t_lws
%type<i> t_colon
%type<i> t_separators
%type<i> t_sp
%type<str> t_ws
%type<i> t_ctl

%type<i> allowed_char_for_text
%type<i> allowed_char_for_token
%type<str> token
%type<str> text
%type<str> ows
%%
allowed_char_for_token:
t_token_char; |
t_digit {
	$$ = '0' + $1;
}; |
t_dot;

token:
allowed_char_for_token {
    snprintf($$, 8192, "%c", $1);
}; |
token allowed_char_for_token {
    snprintf($$, 8192, "%s%c", $1, $2);
};

allowed_char_for_text:
allowed_char_for_token; |
t_separators {
    $$ = $1;
}; |
t_colon {
    $$ = $1;
}; |
t_backslash {
    $$ = $1;
};

ows: {
	$$[0]=0;
}; |
t_sp {
    snprintf($$, 8192, "%c", $1);
}; |
t_ws {
    snprintf($$, 8192, "%s", $1);
};


text: allowed_char_for_text {
    snprintf($$, 8192, "%c", $1);
}; |
text ows allowed_char_for_text {
    snprintf($$, 8192, "%s%s%c", $1, $2, $3);
};

request_line: token t_sp text t_sp text t_crlf {
    //printf("request line: {method: %s, url: %s, version: %s}\n", $1, $3, $5);
    strcpy(parsing_request->http_method, $1);
    strcpy(parsing_request->http_uri, $3);
    strcpy(parsing_request->http_version, $5);
};

request_header_t: token ows t_colon ows text t_crlf {
    //printf("request header: {%s:%s}\n", $1, $5);
    if (parsing_request->header_count == parsing_request->header_capacity) {
        expand_request_header();
    }
    int cur_header_count = parsing_request->header_count;
    RequestHeader *cur_request_header = parsing_request->headers + cur_header_count;
    strcpy(cur_request_header->header_name, $1);
    strcpy(cur_request_header->header_value, $5);
    parsing_request->header_count += 1;
};

request_header: request_header_t; | request_header request_header_t {

};

request_headers: | request_header;

request: request_line request_headers t_crlf {
    return SUCCESS;
};
%%

void set_parsing_options(char *buf, size_t size, Request_C *request)
{
    parsing_buf = buf;
    parsing_offset = 0;
    parsing_buf_size = size;
    parsing_request = request;
}

void expand_request_header() {
    parsing_request->header_capacity *= 2;
    RequestHeader *new_request_header = (RequestHeader *) malloc(sizeof(RequestHeader) * parsing_request->header_capacity);
    for (int i = 0; i < parsing_request->header_count; i++) {
        memcpy(new_request_header + i, parsing_request->headers + i, sizeof(RequestHeader));
    }
    RequestHeader *old_request_headers = parsing_request->headers;
    parsing_request->headers = new_request_header;
    free(old_request_headers);
}