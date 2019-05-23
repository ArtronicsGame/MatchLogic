#ifndef _ICE_MAN
#define _ICE_MAN

#include <bits/stdc++.h>
#include <PlayerData.hpp>
#include <Task.hpp>
#include <PlayerData.hpp>

using namespace std;

class IceMan : public PlayerInfo {
    private:
        vector<Task> tasks;
    public:
        void onAttack(PlayerInfo* playerInfo);
        void AntiAttak(PlayerInfo* playerInfo);
        void OnTick();

        IceMan(string id, string type, int health, int speed, int damage, int attackDelay) : PlayerInfo(id, type, health, speed, damage, attackDelay) {}
};

#endif
