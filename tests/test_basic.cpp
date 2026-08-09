#include <iostream>
#include <atomic>
#include <vector>
#include <cassert>

#include "thread_pool.h"

void test_basic() {
    ThreadPool pool(4, 100);

    //1.测试future返回值

    auto future = pool.submit([]() {
        return 100;
    });

    assert(future.get() == 100);

    //2.测试多任务运行

    std::atomic<int> count{0};
    std::vector<TaskHandle<void>> handles;
    for(int i = 0; i < 100; i++) {
        handles.emplace_back(
            pool.submit([&count]() {
                count++;
            })
        );
    }

    for(auto& h : handles) {
        h.wait();
    }
    assert(count == 100);

    //3.检查任务队列状态
    assert(pool.empty());

    std::cout << "basic test pass\n";
};
int main() {
    test_basic();

    return 0;
}