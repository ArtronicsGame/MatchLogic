#include <MiddleConnection.hpp>

using namespace nlohmann;
using namespace std;

void MiddleConnection::SetIDs(map<string, int> fakeID, map<int, string> realID){
    this->realID = realID;
    this->fakeID = fakeID;
}

void MiddleConnection::sendMessage(json msg, int id){
    stringstream ss;
    ss << msg << endl;
    sendTCP(tcpMap[id], ss.str());
}

void MiddleConnection::sendMessage(UDPPacket packet, int id){
    sendUDPPacket(&udpMap[id].add, udpMap[id].len, packet);
}

void MiddleConnection::broadcastUDP(UDPPacket packet){
    for(pair<int, UDPDest> elem : udpMap){
        sendUDPPacket(&elem.second.add, elem.second.len, packet);
    }
}

void MiddleConnection::OnConnect(string ip, int port, int sock){
    cout << "#"<< sock << " (" << ip << ":" << port << ") " << "Connected" << endl;
}

void MiddleConnection::OnDisconnect(string ip, int port, int sock){
    cout << "#"<< sock << " (" << ip << ":" << port << ") " << "Disconnected" << endl;
    tcpMap.erase(idTMap[sock]);
    idTMap.erase(sock);
}

void MiddleConnection::OnTCPMessage(string msg, string ip, int port, int sock){
    cout << "#"<< sock << " (" << ip << ":" << port << ") " << "Says: " << msg << endl;
    try{
        json message = json::parse(msg);
        if(message["_type"] == "Handshake"){ //TCP Handshake
            string id = message["_info"]["id"];
            if(tcpMap.find(fakeID[id]) != tcpMap.end())
                closeOne(tcpMap[fakeID[id]]);
            tcpMap[fakeID[id]] = sock;
            idTMap[sock] = fakeID[id];

            json resp;
            resp["_type"] = "FakeID";
            resp["_info"]["id"] = fakeID[id];
            sendMessage(resp, fakeID[id]);

            connected++;
            if(connected >= 2 * MATCH_SIZE)
                OnMatchStart();
        }else{
            if(idTMap.find(sock) == idTMap.end())
            {
                sendTCP(sock, "Error: Handshake First");
                return;
            }
            OnMessage(msg, idTMap[sock]);
        }
    }catch(...){
        cout << "Invalid Message" << endl;
    }
}

void MiddleConnection::OnUDPMessage(UDPPacket msg, string ip, int port, sockaddr* addr, int len){            
    UDPDest add;
    add.add = *addr;
    add.len = len;
    if(msg.type == 'H'){ //UDP Handshake
        string id = msg.str;
        udpMap[fakeID[id]] = add;
        idUMap[add] = fakeID[id];
        connected++;
        if(connected >= 2 * MATCH_SIZE)
            OnMatchStart();
    }else{
        if(idUMap.find(add) == idUMap.end())
        {
            UDPPacket packet;
            packet.type = 'H';
            sendUDPPacket(addr, len, packet);
            return;
        }
        OnPacket(msg, idUMap[add]);
    }
}