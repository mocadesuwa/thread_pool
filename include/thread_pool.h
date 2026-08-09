#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <type_traits>
#include <unordered_map>

#include "task.h"
#include "task_handle.h"



class ThreadPool {
private:
    std::mutex mutex;
    std::condition_variable not_full;
    std::condition_variable not_empty;

    std::atomic<bool> stop = false;

    //  线程数组
    std::vector<std::thread> workers;
    //  任务队列
    std::priority_queue<Task, std::vector<Task>, TaskCompare> tasks;
    //  最大限制
    size_t max_queue_size;
    //  id记录
    std::atomic<size_t> task_id{0};
    //  状态表
    std::unordered_map<size_t, std::shared_ptr<TaskControl>> task_states;

public:
    //  关闭方式
    enum class StopMode {
        Graceful,
        DiscardPendingTasks
    };

    ThreadPool(size_t thread_num, size_t queue_size);
    ~ThreadPool();

    template<typename F>
    auto submit(F&& f, int priority = 0);

    void shutdown(StopMode mode);

    //  状态监控
    size_t task_size();
    size_t thread_size();

    bool empty();
    bool full();

    //  状态表查询窗口
    TaskState get_task_state(size_t id);
};

template<typename F>
auto ThreadPool::submit(F&& f, int priority) {
        using return_type = std::invoke_result_t<F>;

        auto task = std::make_shared<std::packaged_task<return_type()>>
            (std::forward<F>(f));

        auto future = task->get_future();

        auto control = std::make_shared<TaskControl>();

        auto id = task_id++;
        {
            std::lock_guard<std::mutex> lock(mutex);
            task_states[id] = control;

            tasks.emplace(
                id,
                priority,
                [task]() {
                    (*task)();
                },
                control);
        }
        not_empty.notify_one();

        return TaskHandle<return_type>(id, std::move(future), control);
    }