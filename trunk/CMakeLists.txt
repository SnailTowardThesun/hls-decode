cmake_minimum_required(VERSION 2.8)
project(flv_decode)
SET(CMAKE_BUILD_TYPE Debug)

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -g -Wall -Wno-unused-variable -pthread -fpermissive")
else( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -O2 -pthread -fopenmp")
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )

set(APP_FUNCTION app/FlvBuffer.cpp app/FlvBuffer.h app/FlvLog.cpp app/FlvLog.h app/FlvSocket.h app/FlvSocket.cpp)

set(KERNEL_FUNCTION kernel/FlvKernel.h kernel/FlvCommonHeaderFile.h)

set(PROTOCOL_FUNCTION  protocol/FlvHeader.cpp protocol/FlvHeader.h protocol/FlvPackage.cpp protocol/FlvPackage.h protocol/FlvHttp.h protocol/FlvHttp.cpp protocol/FlvHttp2.h protocol/FlvHttp2.cpp)

set(SOURCE_FILES main.cpp  FlvDecode.cpp FlvDecode.h ${APP_FUNCTION} ${KERNEL_FUNCTION} ${PROTOCOL_FUNCTION})

set(FLV_BINARY_DIR "./objs/")
set(EXECUTABLE_OUTPUT_PATH ${FLV_BINARY_DIR})
add_executable(flv_decode ${SOURCE_FILES})
