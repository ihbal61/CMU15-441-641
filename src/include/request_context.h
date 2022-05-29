#ifndef _REQUEST_CONTEXT_
#define _REQUEST_CONTEXT_

#include "context.h"
#include "request.h"
#include "util.h"

namespace webserver {
    
class RequestContext : public Context {
public:
    RequestContext(int fd, webserver::Request &_request, ClientInfo &_client_info) 
    : Context(fd), request(_request) {
        client_info.address = _client_info.address;
        client_info.port = client_info.port;
    }

    Request& getRequest();

    ClientInfo& getClientInfo();
    
    int getFileDes();
private:
    Request request;
    ClientInfo client_info;
}; // class RequestContext

} // namespace name
#endif
