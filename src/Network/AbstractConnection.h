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
#include <UDPPacket.h>
#include <Coding.h>
#include <nlohmann/json.hpp>

#define PORT 0
#define BUF_SIZE 1024

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

        AbstractConnection(){

        }

        void createServer(){
            int addrlen, opt = 1;
            struct sockaddr_in address; 
            char buffer[BUF_SIZE];
                                    
            if((tcp = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
            { 
                cout << "TCP Failed" << endl; 
                exit(EXIT_FAILURE); 
            } 

            if((udp = socket(AF_INET , SOCK_DGRAM , 0)) == 0) 
            { 
                cout << "UDP Failed" << endl; 
                exit(EXIT_FAILURE); 
            } 
            
            //set master socket to allow multiple connections , 
            //this is just a good habit, it will work without this 
            if(setsockopt(tcp, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
                sizeof(opt)) < 0 ) 
            { 
                cout << "Setsockopt" << endl; 
                exit(EXIT_FAILURE); 
            } 
            
            //type of socket created 
            address.sin_family = AF_INET; 
            address.sin_addr.s_addr = INADDR_ANY; 
            address.sin_port = htons( PORT ); 
                
            if (bind(tcp, (struct sockaddr *)&address, sizeof(address))<0) 
            { 
                cout << "TCP Bind Failed" << endl; 
                exit(EXIT_FAILURE); 
            } 

            addrlen = sizeof(address); 
            getsockname (tcp, (struct sockaddr *) &address, (socklen_t *) &addrlen);

            if (bind(udp, (struct sockaddr *)&address, sizeof(address))<0) 
            { 
                cout << "UDP Bind Failed" << endl; 
                exit(EXIT_FAILURE); 
            } 

            printf("TCP Listening on port %d \n", ntohs(address.sin_port)); 
            printf("UDP Listening on port %d \n", ntohs(address.sin_port)); 
                
            if (listen (tcp, 10) < 0) 
            { 
                cout << "Listen Failed" << endl; 
                exit(EXIT_FAILURE); 
            }

            FD_ZERO(&fds);
            FD_SET(tcp, &fds);
            FD_SET(udp, &fds);

            max_sd = (tcp > udp ? tcp : udp);
                
            cout << "Initilized Successfully" << endl; 
            reading = true;
            cerr << ntohs(address.sin_port) << endl;
        }

        void readSync(){
            if(!reading){
                cout << "Start Server First" << endl;
                return;
            }

            char buffer[BUF_SIZE];
            while(reading){
                fd_set copy = fds;

                struct timeval tv;
                tv.tv_sec = 2;
                tv.tv_usec = 0;
                int acts = select(max_sd + 1, &copy, nullptr, nullptr, &tv);

                if(acts <= 0){
                    continue;
                }

                if(FD_ISSET(tcp, &copy)){
                    //New Connection
                    sockaddr_in clientAdd;
                    int len = sizeof(struct sockaddr_in);
                    int newSock = accept(tcp, (struct sockaddr *) &clientAdd, (socklen_t *) &len);
                    if(newSock > max_sd)
                        max_sd = newSock;
                    clients.push_back(newSock);
                    FD_SET(newSock, &fds);
                    OnConnect(inet_ntoa(clientAdd.sin_addr), ntohs(clientAdd.sin_port), newSock);
                }

                if(FD_ISSET(udp, &copy)){
                    sockaddr_in clientAdd;
                    int len = sizeof(struct sockaddr_in), msg_len;
                    msg_len = recvfrom(udp, buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr*) &clientAdd, (socklen_t *) &len);
                    UDPPacket packet = decode(buffer, msg_len);
                    OnUDPMessage(packet, inet_ntoa(clientAdd.sin_addr), ntohs(clientAdd.sin_port), (struct sockaddr*) &clientAdd, len);
                }

                for(int i = 0; i < clients.size(); i++){
                    int sock = clients[i];
                    if(FD_ISSET(sock, &copy)){
                        int msg_len;
                        msg_len = read(sock, buffer, BUF_SIZE);
                        if(msg_len == 0){
                            sockaddr_in clientAdd;
                            int len = sizeof(struct sockaddr); 
                            getpeername(sock , (struct sockaddr*) &clientAdd , (socklen_t*) &len);  
                            OnDisconnect(inet_ntoa(clientAdd.sin_addr), ntohs(clientAdd.sin_port), sock);
                            closeOne(sock);  
                        }else if(msg_len > 3){ //Short messages will drop
                            string msg(buffer, msg_len);
                            sockaddr_in clientAdd;
                            int len = sizeof(struct sockaddr); 
                            getpeername(sock , (struct sockaddr*) &clientAdd , (socklen_t*) &len); 
                            OnTCPMessage(msg, inet_ntoa(clientAdd.sin_addr), ntohs(clientAdd.sin_port), sock);
                        }
                    }
                }
            }

            for(int sock: clients){
                close(sock);
            }
            clients.clear();
            close(tcp);
            tcp = 0;
            close(udp);
            udp = 0;
            FD_ZERO(&fds);
        }

        void closeOne(int sock){
            FD_CLR(sock, &fds);
            close(sock);
            clients.erase(remove(clients.begin(), clients.end(), sock), clients.end());
            max_sd = (tcp > udp ? tcp : udp);
            for(int iSock : clients)
                if(iSock > max_sd)
                    max_sd = iSock;
        }

        void closeAll(){
            reading = false;
        }

        void sendTCP(int sock, string msg){
            send(sock, msg.c_str(), msg.length() + 1, 0);
        }

        void broadcastTCP(string msg){
            for(int sock: clients)
                sendTCP(sock, msg);
        }

        void broadcastTCP(json msg){
            stringstream ss;
            ss << msg << endl;
            for(int sock: clients)
                sendTCP(sock, ss.str());
        }

        void sendUDP(sockaddr* addr, int len, char* buf, int m_len){
            sendto(udp, buf, m_len, 0, addr, len);
        }

        void sendUDPPacket(sockaddr* addr, int len, UDPPacket packet){
            char buf[1024];
            int m_len = encode(packet, buf);
            sendUDP(addr, len, buf, m_len);
        }

};


#endif