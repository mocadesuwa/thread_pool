#include <iostream>
#include <chrono>
#include "thread_pool.h"

int main() {
    ThreadPool pool(4);

    for(int i = 0; i < 10; i++) {
        pool.submit(
            [i]() {
                std::cout << "tasks: "
                << i
                << "thread "
                << std::this_thread::get_id()
                << std::endl;

                std::this_thread::sleep_for(std::chrono::seconds(1));
            });
    }

}