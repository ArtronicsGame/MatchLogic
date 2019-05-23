#ifndef _HIT_H
#define _HIT_H

#include <bits/stdc++.h>
#include <ObjectData.hpp>
#include <nlohmann/json.hpp>
#include <Bullet.hpp>

using namespace std;
using namespace nlohmann;

tuple<bool, json> onBullet(Bullet* bullet, ObjectData* other);

#endif