#include <iostream>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <thread>

#include <cstdio>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>

#include "../cameraOS/cameraOS.cpp"
#include "loraPackets/sendPayload.h"

using namespace std;

void startPayload() {
    sendPayload payloadSender;
    payloadSender.start();
}

void setup()
{
//    thread cameraOS = thread(cameraOS::start);
//    cameraOS.detach();

    thread payload = thread(startPayload);
    payload.detach();
}

void loop()
{
    delay(1000);
}

int main()
{
    cout << "Iniciando wiringPI" << endl;
    if (wiringPiSetup() < 0) {
        cout << "setup wiring pi failed" << endl;
        return 1;
    }
    setup();

    while (1) {
        loop();
    }

    return 0;
}