#include <bits/stdc++.h>
#include <thread>
#include <Box2D.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <nlohmann/json.hpp>
#include "ObjectData.h"
#include "Utils/AtomicQueue.h"
#include "Utils/MapReader.h"
#include "Network/MiddleConnection.h"
#include "Utils/ToJson.h"

using namespace std;
using namespace chrono;
using namespace nlohmann;

class Match : public MiddleConnection {
    private:
		const int MATCH_SIZE = 1;

		b2World* _world;

		float32 _timeStep = 1.0f / 50.0f;
		int32 _velocityIterations = 6;
		int32 _positionIterations = 2;

		int _gamePeriod = 1 * 60 * 60; // Min * Sec/Min * FPS

		AtomicQueue<json> events;

		map<int, b2Body*> heroes;
		map<int, Heroes> heroTypes;

		thread* logic;

    public:
        Match() {
			map<int, string> realID;
			map<string, int> fakeID;
			map<int, bool> check;

			for(int i = 0; i < MATCH_SIZE; i++){
				string rid, heroType;
				int id;
				do {
					id = rand() % 10000;
				} while(check[id]);
				cin >> rid >> heroType;
				realID[id] = rid;
				fakeID[rid] = id;
				heroTypes[id] = getHeroEnum(heroType);
			}

			SetIDs(fakeID, realID);
			
			string mapType;
			cin >> mapType;
			_world = getMap(getMapEnum(mapType));

            createServer();
            readSync();
        }

        void OnMessage(string msg, int id){
			events.push(json::parse(msg));
        }

		void OnPacket(UDPPacket packet, int id){
			events.push(toJson(packet, id));
		}

		void OnMatchStart(){
			for (map<int,Heroes>::iterator it = heroTypes.begin(); it!=heroTypes.end(); ++it)
			{
				b2Body* hero = getHero(it->second, _world);
				heroes[it->first] = hero;

				json response;
				response["_type"] = "HeroID";
				response["_info"]["HeroID"] = ((ObjectData*)hero->GetUserData())->id;

				sendMessage(response, it->first);
			}
			broadcastTCP(getWorldJSON());
			logic = new thread(&Match::run, this);
		}

		void EndMatch(){
			json resp;
			resp["_type"] = "MatchEnd";
			resp["_info"]["stuff"] = 2;
			stringstream ss;
			ss << resp << endl;
			broadcastTCP(ss.str());
			closeAll();
		}

		//Game Section


		string getWorldJSON(){
			stringstream ss;
			b2Body* it = _world->GetBodyList();
			while(it != nullptr){
				json message;
				ObjectData *temp = (ObjectData *)it->GetUserData();
				message["_type"] = "NewItem";
				message["_info"]["objName"] = temp->shapeName;
				message["_info"]["elemName"] = temp->id;
				message["_info"]["X"] = to_string(it->GetPosition().x);
				message["_info"]["Y"] = to_string(it->GetPosition().y);
				message["_info"]["Angle"] = to_string(it->GetAngle());
				ss << message << endl;
				it = it->GetNext();
			}
			return ss.str();
		}

		void frame(){
			b2Body* it = _world->GetBodyList();
			stringstream ss;
			bool flag = false;
			while(it != nullptr){
				if(it->GetType() == b2_dynamicBody){
					UDPPacket packet;
					packet.type = 'U';
					packet.fList.push_back(it->GetPosition().x);
					packet.fList.push_back(it->GetPosition().y);
					packet.fList.push_back(it->GetAngle());
					packet.str = ((ObjectData*)it->GetUserData())->id;
					broadcastUDP(packet);
				}
				it = it->GetNext();
			}
		}

		void run() {
			this_thread::sleep_for(milliseconds(1000));
			cout << "GameStarted " << endl;
			milliseconds lastTime;
			for(int i = 0; i < _gamePeriod; i++){
				json event;
				while(events.pop(event)){
					//TODO: Perform New Events
					if(event["_type"] == "HeroMove"){
						b2Vec2 dir;
						float speed = event["_info"]["speed"];
						dir.x = event["_info"]["x"];
						dir.y = event["_info"]["y"];

						int id = event["_info"]["id"];

						heroes[id]->SetTransform(heroes[id]->GetPosition(), atan2f(dir.y, dir.x));

						dir.x *= speed * 6;
						dir.y *= speed * 6;

						heroes[id]->SetLinearVelocity(dir);
					}
					else if(event["_type"] == "HeroAttack")
					{
						
					}
				}
				


				lastTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
				_world->Step(_timeStep, _velocityIterations, _positionIterations);

				frame();

				milliseconds now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
				milliseconds waitTime = milliseconds((int)(_timeStep * 1000)) - (now - lastTime);

				this_thread::sleep_for(waitTime);

			}
			EndMatch();
		}
};

int main() {
	Match m;
    return 0;
}

