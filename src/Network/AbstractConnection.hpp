#ifndef _ABSTRACT_CONNECTION
#define _ABSTRACT_CONNECTION

#include <bits/stdc++.h>
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> 
#include <UDPPacket.hpp>
#include <Coding.hpp>
#include <nlohmann/json.hpp>

using namespace std;
using namespace nlohmann;

class AbstractConnection{
    protected:
        int tcp, udp, max_sd;
        fd_set fds; 
        vector<int> clients;
        bool reading = false; 

    public:

        virtual void OnConnect(string ip, int port, int sock){}
        virtual void OnDisconnect(string ip, int port, int sock){}
        virtual void OnTCPMessage(string msg, string ip, int port, int sock){}
        virtual void OnUDPMessage(UDPPacket msg, string ip, int port, sockaddr* addr, int len){}

        AbstractConnection(){}

        void createServer();
        void readSync();

        void closeOne(int sock);
        void closeAll();

        void sendTCP(int sock, string msg);
        void broadcastTCP(string msg);
        void broadcastTCP(json msg);
        void sendUDP(sockaddr* addr, int len, char* buf, int m_len);
        void sendUDPPacket(sockaddr* addr, int len, UDPPacket packet);

};


#endif