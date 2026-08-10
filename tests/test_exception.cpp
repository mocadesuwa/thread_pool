#include <iostream>
#include <cassert>

#include "thread_pool.h"

void test_exception() {
    ThreadPool pool(1, 100);

    auto handle = pool.submit([]() {
        throw std::runtime_error("error");
    });

    handle.wait();

    auto state = handle.state();
    std::cout << "state: "
              << static_cast<int>(state)
              << std::endl;

    try {
        handle.get();

        assert(false);
    }
    catch(const std::runtime_error& e) {
        std::cout << "catch exception: "
                  << e.what()
                  << std::endl;
    }

    std::cout << "test_exception pass\n";
}

int main() {
    test_exception();

    return 0;
}