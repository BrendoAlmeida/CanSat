#include "cameraOS.h"
#include <cstdlib>

using namespace std;

void cameraOS::tiraFoto() {
    system("python cameraOS/start.py");
}

void cameraOS::start() {
    system("python cameraOS/record.py");
}