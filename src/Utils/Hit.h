#ifndef _HIT_H
#define _HIT_H

#include <bits/stdc++.h>
#include <Box2D.h>
#include <ObjectData.h>
#include <nlohmann/json.hpp>

using namespace std;
using namespace nlohmann;

template <typename Of, typename What>
inline bool instanceof(What *w)
{
    return dynamic_cast<Of*>(w) != 0;
}

tuple<bool, json> onBullet(ObjectData* bullet, ObjectData* other){
    bullet->type = "Del";
    if(instanceof<PlayerInfo>(other)) {
        PlayerInfo* i = (PlayerInfo*) other;
        i->health--;
        if(i->health == 0){
            //Respawn
        }
        json message;
        message["_type"] = "Damage";
        message["_info"]["heroID"] = i->id;
        return {true, message};
    }

    return {false, nullptr};
}

#endif