#include <iostream>
#include <cassert>
#include <atomic>

#include "thread_pool.h"

void test_shutdown() {
    ThreadPool pool(1, 10);

    std::atomic<int> grace_count{0};

    for(int i = 0; i < 100; i++) {
        pool.submit([&]() {
            grace_count++;
        });
    }

    pool.shutdown(ThreadPool::StopMode::Graceful);

    assert(grace_count == 100);


    std::atomic<int> discard_count{0};

    for(int i = 0; i < 100; i++) {
        pool.submit([&]() {
            discard_count++;
        });
    }

    pool.shutdown(ThreadPool::StopMode::DiscardPendingTasks);

    assert(discard_count < 100);

    std::cout << "test_shutdown pass\n";
}


int main() {
    test_shutdown();

    return 0;
}