#ifndef _THREADPOOL_
#define _THREADPOOL_

#include <pthread.h>
#include <list>
#include <mutex>
#include <semaphore.h>
#include <fcntl.h>
#include <glog/logging.h>
#include <sys/types.h>
#include "http_handler.h"

namespace webserver
{

class ThreadPool {
public:
    ThreadPool(size_t _nthreads, size_t _max_requests);

    bool submit(HttpHandler *request);

    void run();

    void stop();

    static void* worker_func(void *argv);

    ~ThreadPool();
private:
    size_t nthreads;
    size_t max_requests;
    pthread_t *threads;
    std::list<HttpHandler *> worker_queue;
    std::mutex lock;
    bool stoped;
    sem_t sem;
}; // class ThreadPool
} // namespace webserver
#endif