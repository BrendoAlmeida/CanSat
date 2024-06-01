#include <iostream>
#include <wiringPi.h>
#include <thread>
#include "../cameraOS/cameraOS.cpp"
#include "loraPackets/packets.cpp"

using namespace std;

void setup() {
    cout << "Hello World" << endl;
    thread cameraOS = thread(cameraOS::start);
    cameraOS.detach();
}

void loop() {
    delay(1000);
    packets::splitPackets();
}

int main()
{
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