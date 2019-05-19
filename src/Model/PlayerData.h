#ifndef _PLAYER_DATA
#define _PLAYER_DATA

#include <bits/stdc++.h>
#include <Box2D.h>
#include <ObjectData.h>

using namespace std;

class PlayerInfo : public ObjectData {
    public:
        int health = 150;
        int speed = 13;
        int damage = 8;
        int attackDelay = 800;

    public:
        virtual void onAttack(b2Body* body){}
        virtual void Tick(){}

        PlayerInfo(string id, string type) : ObjectData(id, type) {}
        PlayerInfo(string id, string type, int health, int speed, int damage, int attackDelay) : ObjectData(id, type), health (health), speed(speed), damage(damage), attackDelay(attackDelay) {}
    
};

#endif