#include "threadpool.h"

namespace webserver
{

void* ThreadPool::worker_func(void *argv) {
    ThreadPool *threadpool = (ThreadPool *) argv;
    threadpool->run();
    return threadpool;
}

ThreadPool::ThreadPool(size_t _nthreads, size_t _max_requests) {
    nthreads = _nthreads;
    max_requests = _max_requests;
    threads = new pthread_t[_nthreads];
    sem_init(&sem, 0, 0);
    stoped = false;
    for (size_t i = 0; i < nthreads; i++) {
        int ret = pthread_create(threads + i, NULL, worker_func, this);
        if (ret != 0) {
            LOG(ERROR) << "create thread failed: "<< errno;
            delete []threads;
            throw std::exception();
        }
    }
}

ThreadPool::~ThreadPool() {
    delete [] threads;
    sem_destroy(&sem);
}

void ThreadPool::stop() {
    stoped = true;
}

void ThreadPool::run() {
    while(!stoped) {
        sem_wait(&sem);
        lock.lock();
        if (worker_queue.size() == 0) {
            lock.unlock();
            continue;
        }
        HttpHandler *request = worker_queue.front();
        worker_queue.pop_front();
        lock.unlock();
        request->process();
    }
}

bool ThreadPool::submit(HttpHandler *request) {
    lock.lock();
    if (worker_queue.size() >= max_requests) {
        return false;
    }
    worker_queue.push_back(request);
    lock.unlock();
    sem_post(&sem);
    return true;
}
} // namespace webserver
