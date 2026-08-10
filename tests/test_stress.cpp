#include <iostream>
#include <cassert>
#include <atomic>
#include <vector>

#include "thread_pool.h"

void test_stress() {
    ThreadPool pool(8, 1000);

    std::vector<TaskHandle<void>> handles;
    std::atomic<int> count{0};

    for(int i = 0; i < 10000; i++) {
        handles.push_back(
            pool.submit([&count]() {
                std::this_thread::sleep_for(std::chrono::microseconds(
                    rand() % 100
                ));

                count++;
            },rand() % 100)
        );
    }

    for(auto& h : handles) {
        h.wait();
    }

    assert(count == 10000);

    std::cout << "test_strss pass\n";
}

int main() {
    test_stress();

    return 0;
}