#ifndef _MAP_READER
#define _MAP_READER

#include <Box2D.h>
#include <bits/stdc++.h>
#include <nlohmann/json.hpp>
#include <ObjectData.hpp>
#include <PlayerData.hpp>

using namespace std;
using namespace nlohmann;

enum Items{
    BULLET = 0,
    UNKNOWN_ITEM = -1
};

enum Maps{
    DEFAULT = 0, 
    UNKNOWN_MAP = -1
};

enum Heroes{
    TANK = 1,
    WIZARD = 0, 
    UNKNOWN_HERO = -1
};

const vector<string> explode(const string& s, const char& c);
b2World* getMap(Maps map);
b2Body* getHero(Heroes hero, b2World *world);
b2Body* spawn(Items item, b2Vec2 pos, float rotation, b2World* world);
Heroes getHeroEnum(string name);
Maps getMapEnum(string name);

#endif