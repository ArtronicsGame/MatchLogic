#ifndef _CODING_H
#define _CODING_H

#include <bits/stdc++.h>
#include <iostream>
#include <UDPPacket.hpp>

using namespace std;

int encode(UDPPacket packet, char *data);
UDPPacket decode(char* sdata, int length);

#endif