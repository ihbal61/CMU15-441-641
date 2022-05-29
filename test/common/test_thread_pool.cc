#include <gtest/gtest.h>
#include "threadpool.h"

TEST(TEST_THREAD_POOL, TEST_ADD_TASK) {
    webserver::ThreadPool threadpool(6, 100);
    threadpool.run();
}