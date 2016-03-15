cmake_minimum_required(VERSION 2.8)
project(hls_decode)
SET(CMAKE_BUILD_TYPE Debug)

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -g -Wall -Wno-unused-variable -pthread -fpermissive")
else( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -O2 -pthread -fopenmp")
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )

set(APP_FUNCTION app/ts_file_distribute.cpp app/ts_file_distribute.h app/log.h app/log.cpp app/ts_hls_distribute.h app/ts_hls_distribute.cpp)

set(KERNEL_FUNCTION kernel/hls_decode_kernel.h)

set(PROTOCOL_FUNCTION protocol/http.cpp protocol/http.h protocol/ts_packet.h protocol/ts_packet.cpp protocol/m3u8file.cpp protocol/m3u8file.h)
set(SOURCE_FILES main.cpp ${APP_FUNCTION} ${KERNEL_FUNCTION} ${PROTOCOL_FUNCTION})

set(FLV_BINARY_DIR "./objs/")
set(EXECUTABLE_OUTPUT_PATH ${FLV_BINARY_DIR})
add_executable(hls_decode ${SOURCE_FILES})
