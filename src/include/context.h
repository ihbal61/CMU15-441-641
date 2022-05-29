#ifndef _CONTEXT_
#define _CONTEXT_

namespace webserver {

class Context {
public:

    Context(int _fd) : fd(_fd) {}

    int getFd();
private:
    int fd;
}; // class context

}
#endif