#include <iostream>
#include <vector>
#include <future>

#include "thread_pool.h"

void test_priority() {
    ThreadPool pool(1, 100);
    //  占位任务
    std::promise<void> start;

    auto blocker = start.get_future();

    auto first = pool.submit(
        [&]() {
            blocker.wait();
        }
    );

    std::vector<TaskHandle<void>> handles;

    for(int i = 0; i < 10; i++) {
        handles.emplace_back(
            pool.submit(
                [i]() {
                    std::cout << "execute priority: " << i << std::endl;
                },i
            )
        );
    }

    start.set_value();
    first.wait();

    for(auto& h : handles) {
        h.wait();
    }

    std::cout << "test priority pass\n";
}

int main() {
    test_priority();

    return 0;
}