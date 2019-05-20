#ifndef _TO_JSON
#define _TO_JSON

#include <bits/stdc++.h>
#include <nlohmann/json.hpp>
#include <ObjectData.hpp>
#include <Box2D.h>
#include <UDPPacket.hpp>

using namespace std;
using namespace nlohmann;

json toJson(UDPPacket packet, int id);
string toString(json message);

namespace JSON {
    json newItem(ObjectData* data, b2Vec2 pos, float32 rotation);
    json startGame();
    tuple<UDPPacket, json> updatePosition(b2Body* body, ObjectData* data);
    json deleteItem(b2Body* body, ObjectData* data, b2World* world);
};

#endif