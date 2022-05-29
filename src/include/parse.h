#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0

//Header field
typedef struct
{
	char header_name[4096];
	char header_value[4096];
} RequestHeader;

//HTTP Request Header
typedef struct
{
	char http_method[50];
	char http_version[50];
	char http_uri[4096];
	RequestHeader *headers;
	int header_count;
	int header_capacity;
} Request_C;
