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
                    task.control->state.store(
                        TaskState::Running
                    );

                    try {
                        task();

                        task.control->state.store(
                            TaskState::Finished
                        );
                    }
                    catch(...) {
                        task.control->state.store(
                            TaskState::Failed
                        );
                    }
                }
            });
    }
}

ThreadPool::~ThreadPool() {
    shutdown(StopMode::Graceful);
}

// 任务关闭
void ThreadPool::shutdown(StopMode mode) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        stop = true;

        if(mode == StopMode::DiscardPendingTasks) {
            while(!tasks.empty()) {
                auto& task = tasks.top();

                task.control->state.store(
                    TaskState::Cancelled
                );

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


//  状态监控
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

//  状态查询
TaskState ThreadPool::get_task_state(size_t id) {
    std::lock_guard<std::mutex> lock(mutex);

    auto it = task_states.find(id);

    if(it == task_states.end()) {
        throw std::runtime_error("task not found");
    }

    return it->second->state.load();


    // auto control = task_states.at(id);
    // return control->state.load();
}