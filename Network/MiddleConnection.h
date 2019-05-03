#ifndef _MIDDLE_CONNECTION
#define _MIDDLE_CONNECTION

#include "AbstractConnection.h"
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace std;

struct UDPDest {
    sockaddr add;
    int len;

    bool operator<(const UDPDest& o) const{
        return ((sockaddr_in*)&add)->sin_port < ((sockaddr_in*)&o.add)->sin_port;
    }
};

class MiddleConnection : public AbstractConnection {
    protected:
        const int MATCH_SIZE = 2;
        int connected = 0;

    protected:
        map<int, string> realID;
        map<string, int> fakeID;

        //Keys: Temp Ids, Values: Sockets/Addr
        map<int, int> tcpMap;
        map<int, UDPDest> udpMap;

        //Keys: Sockets/Addr, Values: Temp Ids
        map<UDPDest, int> idUMap;
        map<int, int> idTMap;

    public:
        void SetIDs(map<string, int> fakeID, map<int, string> realID){
            this->realID = realID;
            this->fakeID = fakeID;
        }

        virtual void OnMessage(string msg, int id){}
        virtual void OnPacket(UDPPacket packet, int id){}
        virtual	void OnMatchStart(){}

        void sendMessage(json msg, int id){
            stringstream ss;
            ss << msg << endl;
            sendTCP(tcpMap[id], ss.str());
        }

        void sendMessage(UDPPacket packet, int id){
            sendUDPPacket(&udpMap[id].add, udpMap[id].len, packet);
        }

        void broadcastUDP(UDPPacket packet){
            for(pair<int, UDPDest> elem : udpMap){
                sendUDPPacket(&elem.second.add, elem.second.len, packet);
            }
        }

        void OnConnect(string ip, int port, int sock){
            cout << "#"<< sock << " (" << ip << ":" << port << ") " << "Connected" << endl;
        }

        void OnDisconnect(string ip, int port, int sock){
            cout << "#"<< sock << " (" << ip << ":" << port << ") " << "Disconnected" << endl;
            tcpMap.erase(idTMap[sock]);
            idTMap.erase(sock);
        }

        void OnTCPMessage(string msg, string ip, int port, int sock){
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
        
        void OnUDPMessage(UDPPacket msg, string ip, int port, sockaddr* addr, int len){            
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
};

#endif