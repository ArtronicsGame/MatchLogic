#include <IceMan.hpp>

using namespace std;

void IceMan::onAttack(PlayerInfo* playerInfo){
    playerInfo->speed *= 0.888f;
    playerInfo->health -= damage;
    tasks.push_back(Task(120, 0, playerInfo)); // 120 Frame Persistance = 3 Shot/Sec And Reduce Speed By (0.888)^3
    sort(tasks.begin(), tasks.end());
}

void IceMan::AntiAttak(PlayerInfo* playerInfo){
    playerInfo->speed /= 0.888f;
}

void IceMan::OnTick(){
    int count = 0;
    for(int i = 0; i < tasks.size(); i++){
        if(tasks[i].Tick() == 0){
            AntiAttak(tasks[i].playerRef);
            count++;
        }
    }
    tasks.erase(tasks.begin(), tasks.begin() + count);
}
