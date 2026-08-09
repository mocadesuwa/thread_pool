#pragma once
#include <atomic>

enum class TaskState {
    Pending,
    Running,
    Finished,
    Failed,
    Cancelled
};

struct TaskControl {
    std::atomic<TaskState> state;

    TaskControl() : state(TaskState::Pending) {}
};