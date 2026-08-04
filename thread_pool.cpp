#include "thread_pool.h"

ThreadPool::ThreadPool(size_t thread_num) {
    for(size_t i = 0; i < thread_num; i++) {
        workers.emplace_back(
            [this]() {
                while(true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        cv.wait(lock, [this]() {return stop || !tasks.empty();});

                        if(stop && tasks.empty()) {
                            return ;
                        }
                        task = tasks.front();
                        tasks.pop();
                    }
                    task();
                }
            });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        stop = true;
    }

    cv.notify_all();

    for(auto& worker : workers) {
        worker.join();
    }
}
