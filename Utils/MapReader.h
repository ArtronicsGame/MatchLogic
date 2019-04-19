#ifndef _MAP_READER
#define _MAP_READER

#include <Box2D.h>
#include <bits/stdc++.h>
#include <nlohmann/json.hpp>
#include "../ObjectData.h"

using namespace std;
using namespace nlohmann;

static int co = 0;

string _heroAdd[] = {
    "/home/centos/Maps/Wizard.txt"
};

string _mapAdd[] = {
    "/home/centos/Maps/DefaultMap.txt"
};

enum Maps{
    DEFAULT = 0, 
    UNKNOWN_MAP = -1
};

enum Heroes{
    WIZARD = 0, 
    UNKNOWN_HERO = -1
};

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

b2World* getMap(Maps map){
    ifstream ifs(_mapAdd[map]);
    string content( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );

    json jMap = json::parse(content);
    json bodies = jMap["Bodies"];

    b2Vec2 gravity(jMap["Gravity"]["X"], jMap["Gravity"]["Y"]);
    
    b2World* world = new b2World(gravity);

    for (json::iterator it = bodies.begin(); it != bodies.end(); ++it) {
        json jBody = *it;
        b2BodyDef bodyDef;
        bodyDef.position.Set(jBody["Position"]["X"], jBody["Position"]["Y"]);
        bodyDef.angle = jBody["Angle"];
        bodyDef.type = (jBody["Dynamic"] ? b2_dynamicBody : b2_staticBody);
        bodyDef.userData = new ObjectData(jBody["Name"], jBody["Type"]);
        b2Body* body = world->CreateBody(&bodyDef);

        json polygons = jBody["Polygons"];
        json circles = jBody["Circles"];
        json chains = jBody["Chains"];

        //Iterate Polygons
        for(json::iterator sit = polygons.begin(); sit != polygons.end(); ++sit){
            json jShape = *sit;
            json jNodes = jShape["Nodes"];
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

        //Iterate Circles
        for(json::iterator sit = circles.begin(); sit != circles.end(); ++sit){
            json jShape = *sit;
            
            b2CircleShape shape;
            shape.m_p.Set(jShape["Position"]["X"], jShape["Position"]["Y"]);
            shape.m_radius = jShape["Radius"];

            b2FixtureDef fixDef;
            fixDef.density = 100;
            fixDef.friction = 0.1;
            fixDef.shape = &shape;

            body->CreateFixture(&fixDef);
        }

        // //Iterate Chains
        for(json::iterator sit = chains.begin(); sit != chains.end(); ++sit){
            json jShape = *sit;
            json jNodes = jShape["Nodes"];
            int count = jNodes.size();
            b2Vec2 vertics[count];

            int i = 0;
            for (json::iterator nit = jNodes.begin(); nit != jNodes.end(); ++nit) {
                json jNode = *nit;
                vertics[i++].Set(jNode["X"], jNode["Y"]);
            }
            b2ChainShape shape;
            shape.CreateChain(vertics, count);

            b2FixtureDef fixDef;
            fixDef.density = 100;
            fixDef.friction = 0.1;
            fixDef.shape = &shape;

            body->CreateFixture(&fixDef);
        }
    }

    return world;
}

b2Body* getHero(Heroes hero, b2World *world){
    ifstream ifs(_heroAdd[hero]);
    string content( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );
    
    json jBody = json::parse(content);

    b2BodyDef bodyDef;
    bodyDef.linearDamping = 0.05f;
    bodyDef.angularDamping = 100;
    bodyDef.position.Set(jBody["Position"]["X"], jBody["Position"]["Y"]);
    bodyDef.angle = jBody["Angle"];
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = new ObjectData("Hero " + to_string(++co), jBody["Type"]);
    b2Body* body = world->CreateBody(&bodyDef);

    json polygons = jBody["Polygons"];
    json circles = jBody["Circles"];
    json chains = jBody["Chains"];

    //Iterate Polygons
    for(json::iterator sit = polygons.begin(); sit != polygons.end(); ++sit){
        json jShape = *sit;
        json jNodes = jShape["Nodes"];
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

    //Iterate Circles
    for(json::iterator sit = circles.begin(); sit != circles.end(); ++sit){
        json jShape = *sit;
        
        b2CircleShape shape;
        shape.m_p.Set(jShape["Position"]["X"], jShape["Position"]["Y"]);
        shape.m_radius = jShape["Radius"];

        b2FixtureDef fixDef;
        fixDef.density = 100;
        fixDef.friction = 0.1;
        fixDef.shape = &shape;

        body->CreateFixture(&fixDef);
    }

    // //Iterate Chains
    for(json::iterator sit = chains.begin(); sit != chains.end(); ++sit){
        json jShape = *sit;
        json jNodes = jShape["Nodes"];
        int count = jNodes.size();
        b2Vec2 vertics[count];

        int i = 0;
        for (json::iterator nit = jNodes.begin(); nit != jNodes.end(); ++nit) {
            json jNode = *nit;
            vertics[i++].Set(jNode["X"], jNode["Y"]);
        }
        b2ChainShape shape;
        shape.CreateChain(vertics, count);

        b2FixtureDef fixDef;
        fixDef.density = 100;
        fixDef.friction = 0.1;
        fixDef.shape = &shape;

        body->CreateFixture(&fixDef);
    }

    return body;
}

Heroes getHeroEnum(string name)
{
    if(name == "Wizard"){
        return WIZARD;
    }

    return UNKNOWN_HERO;
}

Maps getMapEnum(string name)
{
    if(name == "Default"){
        return DEFAULT;
    }

    return UNKNOWN_MAP;
}

#endif