#include "thread_pool.h"

ThreadPool::ThreadPool(size_t thread_num, size_t queue_size) {
    for(size_t i = 0; i < thread_num; i++) {
        workers.emplace_back(
            [this]() {
                while(true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        not_empty.wait(lock, [this]() {
                            return stop || !tasks.empty();
                        });

                        if(stop && tasks.empty()) {
                            return;
                        }

                        task = tasks.front();
                        tasks.pop();

                        not_full.notify_one();
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

    not_empty.notify_all();
    not_full.notify_all();

    for(auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}
