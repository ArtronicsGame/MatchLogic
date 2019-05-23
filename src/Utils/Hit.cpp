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

tuple<bool, json> onBullet(Bullet* bullet, ObjectData* other){
    bullet->type = "Del";
    if(instanceof<PlayerInfo>(other)) {
        PlayerInfo* i = (PlayerInfo*) other;
        bullet->ref->onAttack(i);
        if(i->health == 0){
            //Respawn
        }
        json message;
        message["_type"] = "Damage";
        message["_info"]["heroID"] = i->id;
        message["_info"]["health"] = i->health;
        return {true, message};
    }

    return {false, nullptr};
}