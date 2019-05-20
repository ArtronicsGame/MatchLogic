#include <Coding.hpp>

using namespace std;

#define ISIZE sizeof (int)
#define FSIZE sizeof (float)

int encode(UDPPacket packet, char *data) {
    int ia_size = packet.iList.size();
    int fa_size = packet.fList.size();
    int data_size = 3 + ia_size * ISIZE + fa_size * FSIZE + packet.str.length();

    char iasize = ia_size;
    char fasize = fa_size;

    int counter = 0;

    char idata[ia_size][ISIZE];
    char fdata[fa_size][FSIZE];

    data[counter++] = packet.type;
    data[counter++] = iasize;
    data[counter++] = fasize;

    for(int i=0; i < ia_size; i++) {
        memcpy(idata[i], &packet.iList[i], ISIZE);
        for(int j=0; j < ISIZE; j++)
            data[counter++] = idata[i][j];
    }

    for(int i=0; i < fa_size; i++) {
        memcpy(fdata[i], &packet.fList[i], FSIZE);
        for(int j=0; j < FSIZE; j++)
            data[counter++] = fdata[i][j];
    }

    for(int i=0; i < packet.str.length(); i++)
        data[counter++] = packet.str.at(i);

    return counter;
}

UDPPacket decode(char* sdata, int length) {
    UDPPacket ans;
    unsigned char type = sdata[0];
    int ia_size = sdata[1];
    int fa_size = sdata[2];
    int i_index = 3;
    int f_index = i_index + ia_size * ISIZE;
    int s_index = f_index + fa_size * FSIZE;

    int itemp;
    float ftemp;

    for (int i = 0, j = i_index; i < ia_size; i ++, j += ISIZE) {
        memcpy(&itemp, &sdata[j], ISIZE);
        ans.iList.push_back(itemp);
    }

    for (int i = 0, j = f_index; i < fa_size; i ++, j += FSIZE) {
        memcpy(&ftemp, &sdata[j], FSIZE);
        ans.fList.push_back(ftemp);
    }

    string s1 = "";
    for (int i = s_index; i < length; i++)
        s1 += sdata[i];
    ans.str = s1;
    ans.type = type;

    return ans;
}