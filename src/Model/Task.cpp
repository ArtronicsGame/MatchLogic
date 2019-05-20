#include<Task.hpp>

int Task::Tick() {
    return --remTime;
}

bool Task::operator<(const Task& task) const {
    return remTime < task.remTime;
}

