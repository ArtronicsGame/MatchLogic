#ifndef _TASK_H
#define _TASK_H

#include<Box2D.h>

class Task {
    public:
        int remTime;
        int type;
        b2Body* bodyRef;

    public:
        Task(int remTime, int type, b2Body* bodyRef) : remTime(remTime), type(type), bodyRef(bodyRef) {}

        int Tick() {
            return --remTime;
        }

        bool operator<(const Task& task) const {
            return remTime < task.remTime;
        }
};

#endif