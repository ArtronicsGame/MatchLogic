#ifndef _BULLET_HPP
#define _BULLET_HPP

#include <bits/stdc++.h>
#include <Box2D.h>
#include <PlayerData.hpp>

using namespace std;

class Bullet : public ObjectData {
    public:
        PlayerInfo* ref;

    public:
        Bullet(string id, string type) : ObjectData(id, type) {}
        Bullet(string id, string type, PlayerInfo* ref) : ObjectData(id, type), ref(ref) {}
    
};

#endif