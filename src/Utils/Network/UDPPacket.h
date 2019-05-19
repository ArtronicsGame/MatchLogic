#ifndef _UDP_PACKET
#define _UDP_PACKET

#include <bits/stdc++.h>

using namespace std;

class UDPPacket {
    public:
        char type;
        vector<int> iList;
        vector<float> fList;
        string str;
};

#endif