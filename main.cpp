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
#include "Model/PlayerData.h"

using namespace std;
using namespace chrono;
using namespace nlohmann;

class Match : public MiddleConnection, public b2ContactListener {
    private:
		const float32 _timeStep = 1.0f / 50.0f;
		const int32 _velocityIterations = 6;
		const int32 _positionIterations = 2;

		b2World* _world;

		json _actions;
		string _matchId = "N/A";
		bool _notEnded = true;
		vector<json> _frameAction;
		int _gamePeriod = 0.25 * 60 * 50; // Min * Sec/Min * FPS

		AtomicQueue<json> events;

		map<int, b2Body*> heroes;
		map<int, Heroes> heroTypes;

		thread* logic;

    public:
		template <typename Of, typename What>
		inline bool instanceof(What *w)
		{
			return dynamic_cast<Of*>(w) != 0;
		}

		void BeginContact(b2Contact* contact)
		{
			void* userDataA = contact->GetFixtureA()->GetBody()->GetUserData();
			void* userDataB = contact->GetFixtureB()->GetBody()->GetUserData();			
			ObjectData *dataA = (ObjectData *) userDataA;
			ObjectData *dataB = (ObjectData *) userDataB;
			bool bullet = false;

			if(dataA->type == "Bullet"){
				dataA->type = "Del";
				bullet = true;
			}else if(dataB->type == "Bullet"){
				dataB->type = "Del";
				bullet = true;
			}
			
			if(instanceof<PlayerInfo>(dataA) && bullet) {
				PlayerInfo* i = (PlayerInfo*) dataA;
				i->health--;
				if(i->health == 0){
					EndMatch();
					return;
				}
				json message;
				stringstream ss;
				message["_type"] = "Damage";
				message["_info"]["heroID"] = i->id;
				_frameAction.push_back(message);
				ss << message << endl;
				broadcastTCP(ss.str());
			}else if(instanceof<PlayerInfo>(dataB) && bullet) {
				PlayerInfo* i = (PlayerInfo*) dataB;
				i->health--;
				if(i->health == 0){
					EndMatch();
					return;
				}
				stringstream ss;
				json message;
				message["_type"] = "Damage";
				message["_info"]["heroID"] = i->id;
				_frameAction.push_back(message);
				ss << message << endl;
				broadcastTCP(ss.str());
			}
		}

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
			_world->SetContactListener(this);

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
			cin >> _matchId;
			cout << "We Have Match ID: " << _matchId << endl;
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

		void result(){
			vector<pair<int, int>> scores;
			for (map<int,b2Body*>::iterator it = heroes.begin(); it!=heroes.end(); ++it)
			{
				PlayerInfo* i = (PlayerInfo*) it->second->GetUserData();
				scores.push_back(make_pair(i->health, it->first));
			}

			sort(scores.begin(), scores.end());
			reverse(scores.begin(), scores.end());

			vector<string> win, lose;
			for(int i = 0; i < scores.size(); i++){
				json message;
				message["_type"] = "Result";
				if(i < ceil(scores.size()/2.0)){
					win.push_back(realID[scores[i].second]);
					message["_info"]["State"] = "W";
				}else{
					//Lose
					lose.push_back(realID[scores[i].second]);
					message["_info"]["State"] = "L";
				}
				sendMessage(message, scores[i].second);
			}

			json message;
			message["_type"] = "Result";
			message["_info"]["Winners"] = win;
			message["_info"]["Losers"] = lose;
			message["_info"]["Actions"] = json::to_bson(_actions);
			message["_info"]["MatchID"] = _matchId;
			vector<uint8_t> udata = json::to_bson(message);
			char* data = (char*) &udata[0];

			int sock = 0, valread; 
			struct sockaddr_in serv_addr; 
			char buffer[1024] = {0}; 
			sock = socket(AF_INET, SOCK_STREAM, 0);
			memset(&serv_addr, '0', sizeof(serv_addr)); 
		
			serv_addr.sin_family = AF_INET; 
			serv_addr.sin_port = htons(1111); 
			
			// Convert IPv4 and IPv6 addresses from text to binary form 
			inet_pton(AF_INET, "5.253.27.99", &serv_addr.sin_addr);
		
			if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0) 
			{
				cout << "Connected" << endl;
				send(sock , data, udata.size(), 0 ); 
				// valread = read( sock , buffer, 1024); 
				// string ans(buffer);
				// if(string("OK").find("OK") > -1)
				// 	FinalEnd();
			}  
			FinalEnd();
			
		}

		void EndMatch(){
			_notEnded = false;
			result();
		}

		void FinalEnd(){
			json resp;
			resp["_type"] = "MatchEnd";
			resp["_info"]["stuff"] = 2;
			stringstream ss;
			ss << resp << endl;
			broadcastTCP(ss.str());
			closeAll();
			delete _world;
		}

		//Game Section


		string getWorldJSON(){
			stringstream ss;
			b2Body* it = _world->GetBodyList();
			vector<json> items;
			while(it != nullptr){
				if(it->GetUserData() != nullptr){
					json message;
					ObjectData *temp = (ObjectData *)it->GetUserData();
					if(temp->type != "Wall"){
						message["_type"] = "NewItem";
						message["_info"]["objName"] = temp->type;
						message["_info"]["elemName"] = temp->id;
						message["_info"]["X"] = to_string(it->GetPosition().x);
						message["_info"]["Y"] = to_string(it->GetPosition().y);
						message["_info"]["Angle"] = to_string(it->GetAngle());
						items.push_back(message);
						ss << message << endl;
					}
				}
				it = it->GetNext();
			}
			_actions["Init"] = items;
			return ss.str();
		}

		void frame(){
			b2Body* it = _world->GetBodyList();
			stringstream ss;
			bool flag = false;
			while(it != nullptr){
				if(it->GetType() == b2_dynamicBody){
					ObjectData *temp = (ObjectData *)it->GetUserData();
					if(temp->type == "Del"){
						_world->DestroyBody(it);
						stringstream ss;
						json message;
						message["_type"] = "DelItem";
						message["_info"]["id"] = temp->id;
						_frameAction.push_back(message);
						ss << message << endl;
						broadcastTCP(ss.str());
					}else{
						UDPPacket packet;
						packet.type = 'U';
						packet.fList.push_back(it->GetPosition().x);
						packet.fList.push_back(it->GetPosition().y);
						packet.fList.push_back(it->GetAngle());
						packet.str = temp->id;

						json message;
						message["_type"] = "UpdatePos";
						message["_info"]["id"] = temp->id; 
						message["_info"]["X"] = it->GetPosition().x;
						message["_info"]["Y"] = it->GetPosition().y;
						message["_info"]["Rot"] = it->GetAngle();
						_frameAction.push_back(message);
						
						broadcastUDP(packet);
					}
				}
				it = it->GetNext();
			}
		}

		void run() {
			this_thread::sleep_for(milliseconds(1000));
			stringstream ss;
			json message;
			message["_type"] = "Start";
			ss << message << endl;
			broadcastTCP(ss.str());
			cout << "GameStarted " << endl;
			milliseconds lastTime;

			int i;
			for(i = 0; i < _gamePeriod && _notEnded; i++){
				_frameAction.clear();
				preprocess();

				lastTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
				_world->Step(_timeStep, _velocityIterations, _positionIterations);

				frame();

				milliseconds now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
				milliseconds waitTime = milliseconds((int)(_timeStep * 1000)) - (now - lastTime);

				this_thread::sleep_for(waitTime);
				_actions["F" + to_string(i)] = _frameAction;
			}
			_actions["count"] = i;

			if(_notEnded)
				EndMatch();
		}

		void preprocess(){
			json event;
			while(events.pop(event)){
				if(event["_type"] == "HeroMove"){
					b2Vec2 dir;
					float speed = event["_info"]["speed"];
					dir.x = event["_info"]["x"];
					dir.y = event["_info"]["y"];

					int id = event["_info"]["id"];

					heroes[id]->SetTransform(heroes[id]->GetPosition(), atan2f(dir.y, dir.x));
					PlayerInfo* i = (PlayerInfo*) heroes[id]->GetUserData();

					dir.x *= speed * i->speed;
					dir.y *= speed * i->speed;

					heroes[id]->SetLinearVelocity(dir);
				}
				else if(event["_type"] == "Shoot")
				{
					int id = event["_info"]["id"];
					float rotation = heroes[id]->GetAngle();
					b2Vec2 pos = heroes[id]->GetPosition();
					pos.x += 7 * cos(rotation);
					pos.y += 7 * sin(rotation);

					b2Body* bullet = spawn(BULLET, pos, rotation, _world);

					float forceMag = 100000.0f;
					b2Vec2 force(forceMag * cos(rotation), forceMag * sin(rotation));
					bullet->ApplyForceToCenter(force, false);

					stringstream ss;
					json message;
					ObjectData *temp = (ObjectData *)bullet->GetUserData();
					message["_type"] = "NewItem";
					message["_info"]["objName"] = temp->type;
					message["_info"]["elemName"] = temp->id;
					message["_info"]["X"] = to_string(pos.x);
					message["_info"]["Y"] = to_string(pos.y);
					message["_info"]["Angle"] = to_string(rotation);
					_frameAction.push_back(message);
					ss << message << endl;

					broadcastTCP(ss.str());
				}
			}
		}
};

int main() {
	Match m;
    return 0;
}

