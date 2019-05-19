#ifndef _MATCH_H
#define _MATCH_H

#include <bits/stdc++.h>
#include <thread>
#include <InternalHeaders.h>

using namespace std;
using namespace chrono;
using namespace nlohmann;

class Match : public MiddleConnection, public b2ContactListener{
    private:
		const float32 _timeStep = 1.0f / 50.0f;
		const int32 _velocityIterations = 6;
		const int32 _positionIterations = 2;

		b2World* _world;

		int cnt = 0;
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
		virtual void BeginContact(b2Contact* contact);

        Match();

        void OnMessage(string msg, int id);
		void OnPacket(UDPPacket packet, int id);
		void OnMatchStart();
		void EndMatch();
		void FinalEnd();

		//Game Section
		string getWorldJSON();
		void frame();
		void run();
		void preprocess();
		void sendAndStore(json message);
};

#endif