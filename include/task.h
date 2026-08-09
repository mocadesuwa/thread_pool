#pragma once

#include <functional>
#include <memory>
#include "task_state.h"

struct Task {

    size_t id{0};

    int priority{0};

    std::function<void()> func;

    std::shared_ptr<TaskControl> control;

    Task() = default;
    Task(size_t i, int p, std::function<void()> f, std::shared_ptr<TaskControl> c) :
        id(i), priority(p), func(std::move(f)), control(c) {}

    void operator()() {
        func();
    }
};

struct TaskCompare {
    bool operator() (
        const Task& a, const Task& b
    ) const {
        return a.priority < b.priority;
    }
};