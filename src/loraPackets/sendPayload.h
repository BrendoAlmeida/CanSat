//
// Created by brend on 23/07/2024.
//

#ifndef CANSAT_SENDPAYLOAD_H
#define CANSAT_SENDPAYLOAD_H

#include <vector>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <algorithm>

#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iomanip>
#include <sstream>

#include "lora.h"

#define SPI_CHANNEL 0
#define SS_PIN      8
#define DIO0_PIN    4
#define RST_PIN     22

using namespace std;

class sendPayload {
private:
    int udpSocket;
    const int udpPort = 1234;
    LoRa *lora;
    char sensorData[256];


    vector<unsigned char> readImageToBytes(const string& filename);
    vector<vector<unsigned char>> splitBytesIntoPackets(const vector<unsigned char>& bytes);
    vector<string> listFiles(const string& folderPath);
    string getLastFile(const vector<string>& files);
    string getImage();
    void sendImage(string url);

    void getSensorData();
    void sendMsg(string msg);

public:
    void start();
    sendPayload();
};


#endif //CANSAT_SENDPAYLOAD_H
