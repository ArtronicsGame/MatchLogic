#include <bits/stdc++.h>
#include <thread>
#include <Box2D.h>
#include "Utils/AtomicQueue.h"
#include "ObjectData.h"
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

void init(){
	b2Vec2 gravity(0.0f, -40.0f);
	_world = new b2World(gravity);

	for(int i = 0; i < 3; i++){
		b2BodyDef groundDef;
		if(i == 0){
			groundDef.position.Set(-8.0, 8.0);
			groundDef.angle = -0.349;
			groundDef.userData = new ObjectData("Ground1", "Ground");
		}else if (i == 1){
			groundDef.position.Set(11.0, -3.4);
			groundDef.angle = 0.2617;
			groundDef.userData = new ObjectData("Ground2", "Ground");
		}else{
			groundDef.position.Set(-10.0, -10.0);
			groundDef.angle = 0;
			groundDef.userData = new ObjectData("Ground3", "Ground");
		}

		b2Body* ground = _world->CreateBody(&groundDef);
		b2PolygonShape groundBox;
		groundBox.SetAsBox(8.0, 2.0);

		ground->CreateFixture(&groundBox, 0.0);
	}

	for(int i = 0; i < 2; i++){
		b2BodyDef boxDef;
		if(i == 0){
			boxDef.position.Set(-5.0, 16.0);
			boxDef.angle = 0.523;
			boxDef.type = b2_dynamicBody;
			boxDef.userData = new ObjectData("Crate1", "Crate");
		}else{
			boxDef.position.Set(-11.0, 17.0);
			boxDef.angle = -0.323;
			boxDef.type = b2_dynamicBody;
			boxDef.userData = new ObjectData("Crate2", "Crate");
		}
		
		b2Body* box = _world->CreateBody(&boxDef);
		
		b2PolygonShape boxShape;
		boxShape.SetAsBox(2.0, 2.0);
		
		b2FixtureDef boxFix;
		boxFix.density = 100;
		boxFix.friction = 0.1;
		boxFix.shape = &boxShape;

		box->CreateFixture(&boxFix);
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
	init();
	sendInitialState();

	//Play Section
	thread physic(run);
	//TODO: Push Events
	physic.join();
    return 0;
}

