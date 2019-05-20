#ifndef _TYPE_ENUM
#define _TYPE_ENUM

#include <bits/stdc++.h>

using namespace std;

enum EventTypes {
    HERO_SHOOT, 
    HERO_MOVE,
    UNKNOWN
};

EventTypes toEventType(string type);

#endif