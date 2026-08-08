#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <type_traits>
#include <atomic>

//  任务包装器
struct Task {
    std::function<void()> func;
    int priority;
    size_t id;

    Task() : func(nullptr), priority(0), id(0) {}
    Task(std::function<void()> f, int p, size_t i) : func(std::move(f)), priority(p), id(i) {}
    void operator()() {
        if(func) {
            func();
        }
    }
};

struct TaskCompare {
    bool operator() (
        const Task& a, const Task& b
    ) const {
        return a.priority < b.priority;
    }
};


class ThreadPool {
private:
    //  Task生命周期管理
    enum class TaskState {
        pending,
        Running,
        Finished,
        Failed
    };

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
};

template<typename F>
auto ThreadPool::submit(F&& f, int priority) {
        using return_type = std::invoke_result_t<F>;

        auto task = std::make_shared<std::packaged_task<return_type()>>
            (std::forward<F>(f));

        auto result = task->get_future();

        {
            std::unique_lock<std::mutex> lock(mutex);

            not_full.wait(lock, [this]() {
                return stop || tasks.size() < max_queue_size;
            });

            if(stop) {
                throw std::runtime_error("ThreadPool stopped");
            }

            tasks.emplace([task]() {
                (*task)();},
                priority,
                task_id++);
        }
        not_empty.notify_one();

        return result;
    }