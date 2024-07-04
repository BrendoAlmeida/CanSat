#ifndef CANSAT_PACKETS_H
#define CANSAT_PACKETS_H

#include <vector>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <algorithm>

using namespace std;

class packets {
public:
    static vector<unsigned char> readImageToBytes(const string& filename);
    static vector<vector<unsigned char>> splitBytesIntoPackets(const vector<unsigned char>& bytes);
    static void splitPackets();
    static vector<string> listFiles(const string& folderPath);
    static string getLastFile(const vector<string>& files);
};


#endif //CANSAT_PACKETS_H
