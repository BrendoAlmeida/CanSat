#include <iostream>
#include <wiringPi.h>
#include "../cameraOS/cameraOS.cpp"
#include <thread>

using namespace std;

void setup() {
    cout << "Hello World" << endl;
    thread cameraOS = thread(cameraOS::start);
    cameraOS.detach();
}

void loop() {
    delay(1000);
//    thread cameraOS = thread(cameraOS::tiraFoto);
//    cameraOS.detach();
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