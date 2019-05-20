#include <Judge.hpp>

using namespace std;
using namespace nlohmann;

void result(Match* match){
    vector<pair<int, int>> scores;
    for (map<int,b2Body*>::iterator it = match->heroes.begin(); it != match->heroes.end(); ++it)
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
            win.push_back(match->realID[scores[i].second]);
            message["_info"]["State"] = "W";
        }else{
            //Lose
            lose.push_back(match->realID[scores[i].second]);
            message["_info"]["State"] = "L";
        }
        match->sendMessage(message, scores[i].second);
    }

    json message;
    message["_type"] = "Result";
    message["_info"]["Winners"] = win;
    message["_info"]["Losers"] = lose;
    message["_info"]["Actions"] = json::to_bson(match->_actions);
    message["_info"]["MatchID"] = match->_matchId;
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
    
}