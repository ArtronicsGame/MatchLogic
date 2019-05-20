#include <ToJson.hpp>

using namespace std;
using namespace nlohmann;

int cnt = 0;

json toJson(UDPPacket packet, int id){
    json ans;
    if(packet.type == 'U'){
        ans["_type"] = "HeroMove";
        ans["_info"]["speed"] = packet.fList[0];
        ans["_info"]["x"] = packet.fList[1];
        ans["_info"]["y"] = packet.fList[2];
        ans["_info"]["id"] = id;
    }else if(packet.type == 'S'){
        ans["_type"] = "Shoot";
        ans["_info"]["id"] = id;
    }

    return ans;
}

string toString(json message){
    stringstream ss;
    ss << message << endl;
    return ss.str();
}


json JSON::newItem(ObjectData* data, b2Vec2 pos, float32 rotation){
    json message;
    message["_type"] = "Newbodyem";
    message["_info"]["objName"] = data->type;
    message["_info"]["elemName"] = data->id;
    message["_info"]["X"] = to_string(pos.x);
    message["_info"]["Y"] = to_string(pos.y);
    message["_info"]["Angle"] = to_string(rotation);
    return message;
}

json JSON::startGame(){
    json message;
    message["_type"] = "Start";
    return message;
}

tuple<UDPPacket, json> JSON::updatePosition(b2Body* body, ObjectData* data){
    UDPPacket packet;
    packet.type = 'U';
    packet.fList.push_back(body->GetPosition().x);
    packet.fList.push_back(body->GetPosition().y);
    packet.fList.push_back(body->GetAngle());
    packet.iList.push_back(cnt++);
    cnt %= 100000;
    packet.str = data->id;

    json message;
    message["_type"] = "UpdatePos";
    message["_info"]["id"] = data->id; 
    message["_info"]["X"] = body->GetPosition().x;
    message["_info"]["Y"] = body->GetPosition().y;
    message["_info"]["Rot"] = body->GetAngle();

    return {packet, message};
}

json JSON::deleteItem(b2Body* body, ObjectData* data, b2World* world){
    json message;
    world->DestroyBody(body);
    message["_type"] = "DelItem";
    message["_info"]["id"] = data->id;
    return message;
}
