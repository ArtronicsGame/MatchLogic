#include <bits/stdc++.h>
#include <thread>
#include <Box2D.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <nlohmann/json.hpp>
#include "ObjectData.h"
#include "Utils/AtomicQueue.h"
#include "Utils/MapReader.h"
#include "Network/MiddleConnection.h"

using namespace std;
using namespace chrono;
using namespace nlohmann;

class Connection : public MiddleConnection {
    private:
        AtomicQueue<json> *er;

    public:
        Connection(AtomicQueue<json> *events){
			er = events;
            createServer();
            readSync();
        }

        void OnMessage(string msg, string id){
			er->push(json::parse(msg));
        }
};

const int MATCH_SIZE = 1;

b2World* _world;

float32 _timeStep = 1.0f / 50.0f;
int32 _velocityIterations = 6;
int32 _positionIterations = 2;

bool ended = false;
int _gamePeriod = 0.5 * 60 * 60; // Min * Sec/Min * FPS

AtomicQueue<json> events;
Connection* _connection;

map<int, string> realID;
map<string, int> fakeID;
map<int, b2Body*> heroes;
map<int, Heroes> heroTypes;

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
			if(flag)
				ss << ":";
			ss << it->GetPosition().x << ";" << it->GetPosition().y << ";" << it->GetAngle()  << ";" << ((ObjectData*)it->GetUserData())->id;
			flag = true;
		}
		it = it->GetNext();
	}
	string response = ss.str();
	if(!response.empty())
		cerr << response << endl;
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
				string sspeed = event["_info"]["speed"];
				string sx = event["_info"]["x"];
				string sy = event["_info"]["y"];
				
				float speed = stof(sspeed);
				dir.x = stof(sx);
				dir.y = stof(sy);

				int id = stoi((string)event["_info"]["id"]);
				heroes[id]->SetTransform(heroes[id]->GetPosition(), atan2f(dir.y, dir.x));

				dir.x *= speed * 6;
				dir.y *= speed * 6;

				heroes[id]->SetLinearVelocity(dir);
			}
			else if(event["_type"] == "HeroAttack")
			{
				b2Vec2 dir;
				
				dir.x = stof((string) event["_info"]["x"]);
				dir.y = stof((string) event["_info"]["y"]);

				int id = stoi((string)event["_info"]["id"]);
				heroes[id]->SetTransform(heroes[id]->GetPosition(), atan2f(dir.y, dir.x));

				heroes[id]->SetLinearVelocity(dir);
			}
		}
		


		lastTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		_world->Step(_timeStep, _velocityIterations, _positionIterations);

		frame();

		milliseconds now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		milliseconds waitTime = milliseconds((int)(_timeStep * 1000)) - (now - lastTime);

		this_thread::sleep_for(waitTime);

	}
	ended = true;
}

int main() {
	map<int, bool> check;
	for(int i = 0; i < MATCH_SIZE; i++){
		string rid, heroType;
		int id;
		do {
			id = rand();
		} while(!check[id]);
		cin >> rid >> heroType;
		realID[id] = rid;
		fakeID[rid] = id;
		heroTypes[id] = getHeroEnum(heroType);
	}
	
	string mapType;
	cin >> mapType;
	_world = getMap(getMapEnum(mapType));
 
	for (map<int,Heroes>::iterator it = heroTypes.begin(); it!=heroTypes.end(); ++it)
    {
		b2Body* hero = getHero(it->second, _world);
		heroes[it->first] = hero;

		json response;
		response["_type"] = "HeroID";
		response["_info"]["ID"] = realID[it->first];
		response["_info"]["HeroID"] = ((ObjectData*)hero->GetUserData())->id;

		cout << response << endl;
	}

	getWorldJSON();

	thread physic(run);
	
	_connection = new Connection(&events);

	physic.join();
    return 0;
}

