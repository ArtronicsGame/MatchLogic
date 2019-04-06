#ifndef _MIDDLE_CONNECTION
#define _MIDDLE_CONNECTION

#include "AbstractConnection.h"
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace std;

class MiddleConnection : public AbstractConnection {
    protected:
        map<string, int> tcpMap;
        map<string, sockaddr*> udpMap;

        map<sockaddr*, string> idUMap;
        map<int, string> idTMap;

    public:
        virtual void OnMessage(string msg, string id){}

        void sendMessage(json msg, string id){
            stringstream ss;
            ss << msg;
            sendTCP(tcpMap[id], ss.str());
        }

        void sendMessage(char* buf, int len, string id){
            sendUDP(udpMap[id], buf, len);
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
            cout << "#"<< sock << " (" << ip << ":" << port << ") " << "Says: " << msg;
            try{
                json message = json::parse(msg);
                if(message["_type"] == "Handshake"){
                    string id = message["_info"]["_id"];
                    if(tcpMap.find(id) != tcpMap.end())
                        closeOne(tcpMap[id]);
                    tcpMap[id] = sock;
                    idTMap[sock] = id;
                }else{
                    if(idTMap.find(sock) == idTMap.end())
                    {
                        sendTCP(sock, "Handshake First");
                        return;
                    }
                    OnMessage(msg, idTMap[sock]);
                }
            }catch(...){
                cout << "Invalid Message" << endl;
            }
        }
        
        void OnUDPMessage(string msg, string ip, int port, sockaddr* addr){
            cout << "UDP" << " (" << ip << ":" << port << ") " << "Says: " << msg;
        }
};

#endif