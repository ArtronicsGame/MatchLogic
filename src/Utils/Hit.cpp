#include <Hit.hpp>

#include <bits/stdc++.h>
#include <ObjectData.hpp>
#include <nlohmann/json.hpp>
#include <PlayerData.hpp>

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