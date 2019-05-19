#ifndef _ICE_MAN
#define _ICE_MAN

#include <bits/stdc++.h>
#include <PlayerData.h>
#include <Task.h>

using namespace std;

class IceMan : public PlayerInfo {
    public:
        vector<Task> tasks;
    private:

        void onAttack(b2Body* body){
            PlayerInfo* playerInfo = (PlayerInfo*) body->GetUserData();
            playerInfo->speed *= 0.888f;
            tasks.push_back(Task(120, 0, body));
            sort(tasks.begin(), tasks.end());
        }

        void AntiAttak(b2Body* body){
            PlayerInfo* playerInfo = (PlayerInfo*) body->GetUserData();
            playerInfo->speed /= 0.888f;
        }

        void OnTick(){
            int count = 0;
            for(int i = 0; i < tasks.size(); i++){
                if(tasks[i].Tick() == 0){
                    AntiAttak(tasks[i].bodyRef);
                    count++;
                }
            }
            tasks.erase(tasks.begin(), tasks.begin() + count);
        }

        IceMan(string id, string type, int health, int speed, int damage, int attackDelay) : PlayerInfo(id, type, health, speed, damage, attackDelay) {}
};

#endif
