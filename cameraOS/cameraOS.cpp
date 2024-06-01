#include "cameraOS.h"
#include <cstdlib>

using namespace std;

void cameraOS::start() {
    system("python cameraOS/start.py");
}