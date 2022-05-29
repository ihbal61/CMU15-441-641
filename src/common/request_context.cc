#include "request_context.h"

namespace webserver {

Request& RequestContext::getRequest() {
    return request;
} 

ClientInfo& RequestContext::getClientInfo() {
    return client_info;
}
} // namespace webserver
