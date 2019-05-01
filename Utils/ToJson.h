#ifndef _TO_JSON
#define _TO_JSON

#include <bits/stdc++.h>
#include <nlohmann/json.hpp>
#include "UDPPacket.h"

using namespace std;
using namespace nlohmann;

json toJson(UDPPacket packet, int id){
    json ans;
    if(packet.type == 'U'){
        ans["_type"] = "HeroMove";
        ans["_info"]["speed"] = packet.fList[0];
        ans["_info"]["x"] = packet.fList[1];
        ans["_info"]["y"] = packet.fList[2];
        ans["_info"]["id"] = id;
    }else if(packet.type == 'S'){
        ans["_type"] = "Shoot";
        ans["_info"]["id"] = id;
    }

    return ans;
}

#endif