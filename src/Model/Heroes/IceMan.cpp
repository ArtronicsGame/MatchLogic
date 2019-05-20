#include <IceMan.hpp>

using namespace std;

void IceMan::onAttack(b2Body* body){
    PlayerInfo* playerInfo = (PlayerInfo*) body->GetUserData();
    playerInfo->speed *= 0.888f;
    tasks.push_back(Task(120, 0, body));
    sort(tasks.begin(), tasks.end());
}

void IceMan::AntiAttak(b2Body* body){
    PlayerInfo* playerInfo = (PlayerInfo*) body->GetUserData();
    playerInfo->speed /= 0.888f;
}

void IceMan::OnTick(){
    int count = 0;
    for(int i = 0; i < tasks.size(); i++){
        if(tasks[i].Tick() == 0){
            AntiAttak(tasks[i].bodyRef);
            count++;
        }
    }
    tasks.erase(tasks.begin(), tasks.begin() + count);
}
