#ifndef _HIT_H
#define _HIT_H

#include <bits/stdc++.h>
#include <ObjectData.hpp>
#include <nlohmann/json.hpp>

using namespace std;
using namespace nlohmann;

tuple<bool, json> onBullet(ObjectData* bullet, ObjectData* other);

#endif