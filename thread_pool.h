#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {
private:
    std::mutex mutex;
    std::condition_variable cv;
    bool stop;

    //  线程数组
    std::vector<std::thread> workers;
    //  任务队列
    std::queue<std::function<void()>> tasks;
public:
    ThreadPool(size_t thread_num);
    ~ThreadPool();

    void submit(std::function<void()> task);
};