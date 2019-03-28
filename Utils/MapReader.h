#ifndef _MAP_READER
#define _MAP_READER

#include <Box2D.h>
#include <bits/stdc++.h>
#include <nlohmann/json.hpp>
#include "../ObjectData.h"

using namespace std;
using namespace nlohmann;

const vector<string> explode(const string& s, const char& c)
{
	string buff{""};
	vector<string> v;
	
	for(auto n:s)
	{
		if(n != c) buff+=n; else
		if(n == c && buff != "") { v.push_back(buff); buff = ""; }
	}
	if(buff != "") v.push_back(buff);
	
	return v;
}

b2World* readMap(string mapAddress){

    ifstream ifs(mapAddress);
    string content( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );

    json map = json::parse(content);
    json polygonBodies = map["PolygonBodies"];

    b2Vec2 gravity(map["Gravity"]["X"], map["Gravity"]["Y"]);
    
    b2World* world = new b2World(gravity);

    for (json::iterator it = polygonBodies.begin(); it != polygonBodies.end(); ++it) {
        json jBody = *it;
        b2BodyDef bodyDef;
        bodyDef.position.Set(jBody["Position"]["X"], jBody["Position"]["Y"]);
        bodyDef.angle = jBody["Angle"];
        bodyDef.type = (jBody["Dynamic"] ? b2_dynamicBody : b2_staticBody);
        bodyDef.userData = new ObjectData(jBody["Name"], jBody["Type"]);
        b2Body* body = world->CreateBody(&bodyDef);

        json jNodes = jBody["Nodes"];
        int count = jNodes.size();
        b2Vec2 vertics[count];

        int i = 0;
        for (json::iterator nit = jNodes.begin(); nit != jNodes.end(); ++nit) {
            json jNode = *nit;
            vertics[i++].Set(jNode["X"], jNode["Y"]);
        }
        b2PolygonShape shape;
        shape.Set(vertics, count);

        b2FixtureDef fixDef;
        fixDef.density = 100;
        fixDef.friction = 0.1;
        fixDef.shape = &shape;

        body->CreateFixture(&fixDef);
    }

    return world;
}

#endif