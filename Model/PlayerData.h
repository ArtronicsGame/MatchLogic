#ifndef _PLAYER_DATA
#define _PLAYER_DATA

#include <bits/stdc++.h>
#include "../ObjectData.h"

using namespace std;

class PlayerInfo : public ObjectData {
    public:
        int health = 3;
        int speed = 6;

    public:
        PlayerInfo(string id, string type) : ObjectData(id, type) {}
    
};

#endif