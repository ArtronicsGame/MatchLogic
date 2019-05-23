#ifndef _PLAYER_DATA
#define _PLAYER_DATA

#include <bits/stdc++.h>
#include <Box2D.h>
#include <ObjectData.hpp>
#include <PlayerData.hpp>

using namespace std;

class PlayerInfo : public ObjectData {
    public:
        int health = 150;
        int speed = 13;
        int damage = 8; // Per Attack
        int attackDelay = 40; // Unit: Frame, 40Frames = 800ms

    public:
        virtual void onAttack(PlayerInfo* playerInfo){}
        virtual void AntiAttak(PlayerInfo* playerInfo){}
        virtual void Tick(){}

        PlayerInfo(string id, string type) : ObjectData(id, type) {}
        PlayerInfo(string id, string type, int health, int speed, int damage, int attackDelay) : ObjectData(id, type), health (health), speed(speed), damage(damage), attackDelay(attackDelay) {}
};

#endif