#pragma once
#include <future>
#include <memory>

#include "task_state.h"

template<typename T>
class TaskHandle {
private:
    size_t id;

    std::future<T> future;

    std::shared_ptr<TaskControl> control;

public:
    TaskHandle(size_t i, std::future<T> f, std::shared_ptr<TaskControl> c) :
            id(i), future(std::move(f)), control(std::move(c)) {}

    TaskHandle(const TaskHandle&) = delete;
    TaskHandle& operator=(const TaskHandle&) = delete;

    TaskHandle(TaskHandle&&)=default;
    TaskHandle& operator=(TaskHandle&&)=default;

    size_t id_value() const{
        return id;
    }

    TaskState state() const {
        return control->state.load();
    }

    void wait() {
        future.wait();
    }

    T get() {
        return future.get();
    }
};

template<>
class TaskHandle<void> {
private:

    size_t id;

    std::future<void> future;

    std::shared_ptr<TaskControl> control;


public:
    TaskHandle(size_t i, std::future<void> f, std::shared_ptr<TaskControl> c) :
        id(i), future(std::move(f)), control(std::move(c)) {}

    size_t id_value() const
    {
        return id;
    }

    TaskState state() const
    {
        return control->state.load();
    }

    void wait()
    {
        future.wait();
    }

    void get()
    {
        future.get();
    }

};
