#include <EventTypes.hpp>

using namespace std;

EventTypes toEventType(string type){
    if(type == "HeroMove")
        return HERO_MOVE;
    if(type == "Hero")
        return HERO_SHOOT;
    return UNKNOWN;
}