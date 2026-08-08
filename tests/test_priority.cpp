#include <iostream>
#include <vector>
#include <chrono>

#include "thread_pool.h"

void test_priority() {
    ThreadPool pool(1, 100);
    //  占位任务
    pool.submit([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    });

    std::vector<std::future<void>> futures;
    for(int i = 0; i < 20; i++) {
        futures.push_back(pool.submit(
            [i]() {
                std::cout << "execute task priority: " << i << "\n";
            },i)
        );
    }

    for(auto& f : futures) {
        f.get();
    }

    std::cout << "test priority pass\n";
}

int main() {
    test_priority();

    return 0;
}