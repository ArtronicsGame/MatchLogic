#ifndef _MAP_READER
#define _MAP_READER

#include <Box2D.h>
#include <bits/stdc++.h>

using namespace std;

const vector<string> explode(const string& s, const char& c)
{
	string buff{""};
	vector<string> v;
	
	for(auto n:s)
	{
		if(n != c) buff+=n; else
		if(n == c && buff != "") { v.push_back(buff); buff = ""; }
	}
	if(buff != "") v.push_back(buff);
	
	return v;
}

class MapReader{
    public:
        MapReader(b2World world) : world(world){}
        void append(string mapAddress){

            ifstream map(mapAddress);
            string line;
            
            if (map.is_open())
            {
                while ( getline (map,line) )
                {
                    cout << line << '\n';
                }
                map.close();
            }
        }
    private:
        b2World world;
};

#endif