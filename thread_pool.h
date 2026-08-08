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
    void operator()() {
        func();
    }
};


class ThreadPool {
private:
    //  关闭方式
    enum class StopMode {
        Graceful,
        Immediate
    };

    std::mutex mutex;
    std::condition_variable not_full;
    std::condition_variable not_empty;

    std::atomic<bool> stop = false;

    //  线程数组
    std::vector<std::thread> workers;
    //  任务队列
    std::queue<Task> tasks;
    //  最大限制
    size_t max_queue_size;
public:
    ThreadPool(size_t thread_num, size_t queue_size);
    ~ThreadPool();

    template<typename F>
    auto submit(F&& f);

    void shutdown(StopMode mode);

    //  状态监控
    size_t task_size();
    size_t thread_size();

    bool empty();
    bool full();
};

template<typename F>
auto ThreadPool::submit(F&& f) {
        using return_type = std::invoke_result_t<F&&>;

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
                (*task)();
            });
        }
        not_empty.notify_one();

        return result;
    }