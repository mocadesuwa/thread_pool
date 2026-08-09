#include <iostream>
#include <chrono>
#include <cassert>
#include <thread>

#include "thread_pool.h"

void test_task_state() {
    ThreadPool pool(1, 10);

    auto handle = pool.submit([]() {
        std::this_thread::sleep_for(std::chrono::seconds(2));

    });

    //  1.
    auto state1 = handle.state();

    std::cout << "state after submit "
              << static_cast<int>(state1)
              << std::endl;
    assert(state1 == TaskState::Pending ||
           state1 == TaskState::Running);

    //  2.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    auto state2 = handle.state();

    std::cout << "state during execute"
              << static_cast<int>(state2)
              << std::endl;

    assert(state2 == TaskState::Running);

    //  3.
    handle.wait();

    auto state3 = handle.state();

    std::cout << "state after finish: "
              << static_cast<int>(state3)
              << std::endl;

    assert(state3 == TaskState::Finished);

    std::cout << "test_task_state pass\n";
}

int main() {
    test_task_state();

    return 0;
}