#ifndef CANSAT_PACKETS_H
#define CANSAT_PACKETS_H

#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace std {

    class packets {
    public:
        static vector<unsigned char> readImageToBytes(const string& filename);
        static vector<vector<unsigned char>> splitBytesIntoPackets(const vector<unsigned char>& bytes);
        static void joinPackets(vector<vector<unsigned char>> packets);
        static void splitPackets();
    };

}

#endif
