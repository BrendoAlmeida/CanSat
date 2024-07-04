#include <iostream>
#include <wiringPi.h>
#include <thread>
#include "../cameraOS/cameraOS.cpp"
#include "loraPackets/lora.h"

using namespace std;

void setup()
{
//    thread cameraOS = thread(cameraOS::start);
//    cameraOS.detach();

    thread lora = thread(lora::start);
    lora.detach();
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