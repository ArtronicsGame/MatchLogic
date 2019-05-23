#include <MapReader.hpp>

using namespace std;
using namespace nlohmann;

static int co = 0;
nlohmann::json spawnPlaces;

string _heroAdd[] = {
    "/home/centos/Maps/Wizard.json",
    "/home/centos/Maps/Tank.json"
};

string _mapAdd[] = {
    "/home/centos/Maps/DefaultMap.json"
};

string _itemAdd[] = {
    "/home/centos/Maps/Bullet.json"
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
    spawnPlaces = jMap["SpawnPlaces"];

    b2Vec2 gravity(jMap["Gravity"]["X"], jMap["Gravity"]["Y"]);

    b2World* world = new b2World(gravity);
    
    //Set Walls
    float width = jMap["Width"];
    float height = jMap["Height"];

    b2BodyDef wallsDef;
    wallsDef.position.Set(0, 0);
    wallsDef.type = b2_staticBody;
    wallsDef.userData = new ObjectData("Wall", "Wall");
    b2Body* walls = world->CreateBody(&wallsDef);
    b2ChainShape wallsShape;
    b2Vec2 wallsNodes[4];
    wallsNodes[0].Set(0, 0);
    wallsNodes[1].Set(width, 0);
    wallsNodes[2].Set(width, height);
    wallsNodes[3].Set(0, height);
    wallsShape.CreateLoop(wallsNodes, 4);
    walls->CreateFixture(&wallsShape, 1.0);

    for (json::iterator it = bodies.begin(); it != bodies.end(); ++it) {
        json jBody = *it;
        b2BodyDef bodyDef;
        bodyDef.position.Set(jBody["Position"]["X"], jBody["Position"]["Y"]);
        bodyDef.angle = jBody["Angle"];
        bodyDef.bullet = jBody["Bullet"];
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
    bodyDef.position.Set(spawnPlaces[co]["X"], spawnPlaces[co]["Y"]);
    bodyDef.angle = jBody["Angle"];
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = jBody["Bullet"];
    bodyDef.userData = new PlayerInfo("Hero " + to_string(++co), jBody["Type"]);
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

    //Iterate Chains
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

b2Body* spawn(Items item, b2Vec2 pos, float rotation, b2World* world){
     ifstream ifs(_itemAdd[item]);
    string content( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );
    
    json jBody = json::parse(content);

    b2BodyDef bodyDef;
    bodyDef.linearDamping = 0.05f;
    bodyDef.angularDamping = 100;
    bodyDef.position = pos;
    bodyDef.angle = rotation;
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = jBody["Bullet"];
    bodyDef.userData = new Bullet("I" + to_string(++co), jBody["Type"]);
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

    //Iterate Chains
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
    if(name == "Wizard")
        return WIZARD;
    else if(name == "Tank")
        return TANK;

    return UNKNOWN_HERO;
}

Maps getMapEnum(string name)
{
    if(name == "Default")
        return DEFAULT;

    return UNKNOWN_MAP;
}