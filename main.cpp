#include <iostream>
#include <chrono>
#include <atomic>
#include "thread_pool.h"

void test_basic() {
    ThreadPool pool(4, 100);
    std::atomic<int> count{0};
    std::vector<std::future<void>> futures;

    for(int i = 0; i < 100; i++) {
        futures.push_back(pool.submit([&count]() {
            count++;
        }));
    }

    for(auto& f : futures) {
        f.get();
    }

    std::cout << "count = " << count << std::endl;
}

void test_future() {
    ThreadPool pool(4, 100);
    std::vector<std::future<int>> results;

    for(int i = 0; i < 100; i++) {
        results.push_back(pool.submit([i]() {
            return i * i;
        }));
    }

    int sum = 0;
    for(auto& single : results) {
        sum += single.get();
    }
    std::cout << "sum = " << sum << std::endl;

}

void test_exception() {
    ThreadPool pool(4, 100);
    auto future = pool.submit([]() {
        throw std::runtime_error("task error");
        return 0;
    });

    try {
        int val = future.get();
        std::cout << val;
    }
    catch (const std::runtime_error& e) {
        std::cout << "catch " << e.what() << std::endl;
    }
}
void test_performance() {
    ThreadPool pool(4, 100);
    std::vector<std::future<void>> results;

    auto start = std::chrono::steady_clock::now();

    for(int i = 0; i < 8; i++) {
        results.push_back(
            pool.submit([]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        })
        );
    }

    for(auto& result : results) {
        result.get();
    }

    auto end = std::chrono::steady_clock::now();
    auto cost = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "cost time = " << cost << "s" << std::endl;
}

void test_concurrency() {
    ThreadPool pool(4, 100);
    std::atomic<int> running{0};
    std::atomic<int> max_running{0};

    std::vector<std::future<void>> futures;

    for(int i = 0; i < 100; i++) {
        futures.push_back(pool.submit(
            [&]() {
                int current = ++running;

                max_running.store(std::max(max_running.load(), current));

                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                --running;
            }
        ));
    }

    for(auto& f : futures) {
        f.get();
    }

    std::cout << "max concurrency = " << max_running << std::endl;
}

int main() {
    //  测试一 基础性能
    std::cout << "\n基础性能测试" << std::endl;
    test_basic();

    //  测试二 批量future返回
    std::cout << "\n批量future返回测试" << std::endl;
    test_future();

    //  测试三 异常传递
    std::cout << "\n异常传递测试" << std::endl;
    test_exception();

    // 测试四 并发性能
    std::cout << "\n并发性能测试" << std::endl;
    test_performance();

    // 测试五 并发线程数
    std::cout << "\n并发线程数" << std::endl;
    test_concurrency();
    return 0;
}

