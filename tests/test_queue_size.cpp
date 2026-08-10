#include <iostream>
#include <cassert>
#include <chrono>
#include <atomic>

#include "thread_pool.h"

void test_queue_size() {
    ThreadPool pool(1, 2);

    std::promise<void> blocker;

    auto future = blocker.get_future();

    pool.submit([&]() {
        future.wait();
    });

    std::atomic<int> completed_count{0};

    pool.submit([&]() { completed_count++; });
    pool.submit([&]() { completed_count++; });

    auto start = std::chrono::steady_clock::now();
    std::thread overflow_thread([&]() {
        pool.submit([&]() { completed_count++; });
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    blocker.set_value();

    if(overflow_thread.joinable()) {
        overflow_thread.join();
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    assert(duration.count() >= 50);

    std::cout << "test_queue_size pass\n";
}

int main() {
    test_queue_size();

    return 0;
}