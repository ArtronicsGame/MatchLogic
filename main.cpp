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

using namespace std;
using namespace chrono;
using namespace nlohmann;

b2World* _world;

float32 _timeStep = 1.0f / 50.0f;
int32 _velocityIterations = 6;
int32 _positionIterations = 2;

int _gamePeriod = 0.2 * 60 * 60; // Min * Sec/Min * FPS

AtomicQueue<int> events;

void sendInitialState(){
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
		cout << message << endl;
		it = it->GetNext();
	}
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
	cerr << ss.str() << endl;
}

void run() {
	this_thread::sleep_for(milliseconds(1000));
	cout << "GameStarted " << endl;
	milliseconds lastTime;
	for(int i = 0; i < _gamePeriod; i++){
		//TODO: Perform New Events
		lastTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		_world->Step(_timeStep, _velocityIterations, _positionIterations);

		frame();

		milliseconds now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		milliseconds waitTime = milliseconds((int)(_timeStep * 1000)) - (now - lastTime);

		this_thread::sleep_for(waitTime);

	}

}

int main() {

	while(true){
		string input;
		cin >> input;
		if(input == "Start")
			break;
	}
	
	//Init Section
	_world = readMap("/home/centos/Maps/DefaultMap.txt");
	sendInitialState();

	//Play Section
	thread physic(run);
	//TODO: Push Events
	physic.join();
    return 0;
}

