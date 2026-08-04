#include <iostream>
#include <chrono>
#include "thread_pool.h"

int main() {
    ThreadPool pool(4);

    auto result = pool.submit([](){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return 100;
    });

    std::cout << result.get() << std::endl;
}