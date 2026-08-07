#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <type_traits>

class ThreadPool {
private:
    std::mutex mutex;
    std::condition_variable not_full;
    std::condition_variable not_empty;
    bool stop = false;

    //  线程数组
    std::vector<std::thread> workers;
    //  任务队列
    std::queue<std::function<void()>> tasks;
    //  最大限制
    size_t max_queue_size;
public:
    ThreadPool(size_t thread_num, size_t queue_size);
    ~ThreadPool();

    template<typename F>
    auto submit(F&& f);
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