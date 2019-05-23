#ifndef _MIDDLE_CONNECTION
#define _MIDDLE_CONNECTION

#include <AbstractConnection.hpp>
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
        const int MATCH_SIZE = 1;
        int connected = 0;

    public:
        map<int, string> realID;
        map<string, int> fakeID;

        //Keys: Temp Ids, Values: Sockets/Addr
        map<int, int> tcpMap;
        map<int, UDPDest> udpMap;

        //Keys: Sockets/Addr, Values: Temp Ids
        map<UDPDest, int> idUMap;
        map<int, int> idTMap;

    public:
        void SetIDs(map<string, int> fakeID, map<int, string> realID);

        virtual void OnMessage(string msg, int id){}
        virtual void OnPacket(UDPPacket packet, int id){}
        virtual	void OnMatchStart(){}

        void sendMessage(json msg, int id);
        void sendMessage(UDPPacket packet, int id);
        void broadcastUDP(UDPPacket packet);
        void OnConnect(string ip, int port, int sock);
        void OnDisconnect(string ip, int port, int sock);
        void OnTCPMessage(string msg, string ip, int port, int sock);
        void OnUDPMessage(UDPPacket msg, string ip, int port, sockaddr* addr, int len);
};

#endif