#include "thread_pool.h"

ThreadPool::ThreadPool(size_t thread_num, size_t queue_size) : max_queue_size(queue_size) {
    for(size_t i = 0; i < thread_num; i++) {
        workers.emplace_back(
            [this]() {
                while(true) {
                    Task task;

                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        not_empty.wait(lock, [this]() {
                            return stop || !tasks.empty();
                        });

                        if(stop && tasks.empty()) {
                            return;
                        }

                        task = tasks.top();
                        tasks.pop();

                        not_full.notify_one();
                    }
                    task();
                }
            });
    }
}

ThreadPool::~ThreadPool() {
    shutdown(StopMode::Graceful);
}

void ThreadPool::shutdown(StopMode mode) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        stop = true;

        if(mode == StopMode::DiscardPendingTasks) {
            while(!tasks.empty()) {
                tasks.pop();
            }
        }
    }

    not_empty.notify_all();
    not_full.notify_all();

    for(auto& worker : workers) {
        if(worker.joinable()) {
            worker.join();
        }
    }
}

size_t ThreadPool::task_size() {
    std::lock_guard<std::mutex> lock(mutex);

    return tasks.size();
}

size_t ThreadPool::thread_size() {
    return workers.size();
}

bool ThreadPool::empty() {
    std::lock_guard<std::mutex> lock(mutex);

    return tasks.empty();
}

bool ThreadPool::full() {
    std::lock_guard<std::mutex> lock(mutex);

    return tasks.size() >= max_queue_size;
}