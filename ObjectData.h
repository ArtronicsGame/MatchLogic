#ifndef _OBJECT_DATA
#define _OBJECT_DATA

#include<bits/stdc++.h>

using namespace std;

class ObjectData{
    public:
        string id;
        string type;

        ObjectData(string id, string type) : id(id), type(type){}
        virtual ~ObjectData() = default;
};
#endif
