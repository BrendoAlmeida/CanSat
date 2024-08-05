cmake_minimum_required(VERSION 3.27)
project(CanSat)

set(CMAKE_CXX_STANDARD 14)

include_directories(cameraOS)
include_directories(src/loraPackets)

add_executable(CanSat
        cameraOS/cameraOS.cpp
        cameraOS/cameraOS.h
        src/loraPackets/lora.cpp
        src/loraPackets/lora.h
        src/main.cpp)
