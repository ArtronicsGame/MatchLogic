#include <Match.hpp>

#include <bits/stdc++.h>
#include <thread>
#include <InternalHeaders.hpp>
#include <Judge.hpp>

using namespace std;
using namespace chrono;
using namespace nlohmann;


void Match::BeginContact(b2Contact* contact)
{
    void* userDataA = contact->GetFixtureA()->GetBody()->GetUserData();
    void* userDataB = contact->GetFixtureB()->GetBody()->GetUserData();			
    ObjectData *dataA = (ObjectData *) userDataA;
    ObjectData *dataB = (ObjectData *) userDataB;

    if(dataA->type == "Bullet"){
        auto [state, message] = onBullet(dataA, dataB);
        if(state){
            _frameAction.push_back(message);
            broadcastTCP(message);
        }
    }else if(dataB->type == "Bullet"){
        auto [state, message] = onBullet(dataB, dataA);
        if(state){
            _frameAction.push_back(message);
            broadcastTCP(message);
        }
    }
}

Match::Match() {
    map<int, string> realID;
    map<string, int> fakeID;
    map<int, bool> check;
    cout << "Program Started" << endl;
    for(int i = 0; i < MATCH_SIZE; i++){
        string rid, heroType;
        int id;
        do {
            id = rand() % 10000;
        } while(check[id]);
        cin >> rid >> heroType;
        cout << rid << " -> " << heroType << endl;
        realID[id] = rid;
        fakeID[rid] = id;
        heroTypes[id] = getHeroEnum(heroType);
    }

    SetIDs(fakeID, realID);
    
    string mapType;
    cin >> mapType;
    cout << mapType << endl;
    _world = getMap(getMapEnum(mapType));
    _world->SetContactListener(this);

    cout << "Config Done" << endl;

    createServer();
    readSync();
}

void Match::OnMessage(string msg, int id){
    events.push(json::parse(msg));
}

void Match::OnPacket(UDPPacket packet, int id){
    events.push(toJson(packet, id));
}

void Match::OnMatchStart(){
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

void Match::EndMatch(){
    _notEnded = false;
    result(this);
}

void Match::FinalEnd(){
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
string Match::getWorldJSON(){
    stringstream ss;
    b2Body* it = _world->GetBodyList();
    vector<json> items;
    while(it != nullptr){
        if(it->GetUserData() != nullptr){
            json message;
            ObjectData *temp = (ObjectData *)it->GetUserData();
            if(temp->type != "Wall"){
                message = JSON::newItem(temp, it->GetPosition(), it->GetAngle());
                items.push_back(message);
                ss << message << endl;
            }
        }
        it = it->GetNext();
    }
    _actions["Init"] = items;
    return ss.str();
}

void Match::frame(){
    b2Body* it = _world->GetBodyList();
    while(it != nullptr){
        if(it->GetType() == b2_dynamicBody){
            ObjectData *data = (ObjectData *)it->GetUserData();
            if(data->type == "Del"){
                json message = JSON::deleteItem(it, data, _world);
                sendAndStore(message);
            }else{
                auto [packet, message] = JSON::updatePosition(it, data);
                _frameAction.push_back(message);
                broadcastUDP(packet);
            }
        }
        it = it->GetNext();
    }
}

void Match::run() {
    this_thread::sleep_for(milliseconds(1000));
    broadcastTCP(JSON::startGame());
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

void Match::preprocess(){
    json event;
    while(events.pop(event)){
        int id = event["_info"]["id"];
        switch (toEventType(event["_type"]))
        {
            case HERO_MOVE:
                OnEvents::OnHeroMove(event, heroes[id]);
                break;
            case HERO_SHOOT:
                sendAndStore(OnEvents::OnHeroShoot(event, heroes[id], _world));
                break;
        }
    }
}

void Match::sendAndStore(json message){
    _frameAction.push_back(message);
    broadcastTCP(message);
}