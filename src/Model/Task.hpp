#ifndef _TASK_H
#define _TASK_H

#include <Box2D.h>
#include <PlayerData.hpp>

class Task {
    public:
        int remTime;
        int type;
        PlayerInfo* playerRef;

    public:
        Task(int remTime, int type, PlayerInfo* playerRef) : remTime(remTime), type(type), playerRef(playerRef) {}

        int Tick();
        bool operator<(const Task& task) const;
};

#endif