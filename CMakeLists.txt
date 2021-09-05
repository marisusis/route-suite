cmake_minimum_required(VERSION 3.19)
project(route_suite)

# using at least C++17
set(CMAKE_CXX_STANDARD 17)

# conan.io
include(conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(COMMON_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/common")

# Common library for route
add_library(RouteCommon STATIC
            common/pipes/Pipe.h
            common/pipes/Pipe.cpp
            common/pipes/PipeServer.h
            common/pipes/PipeServer.cpp
            common/pipes/PipeClient.h
            common/pipes/PipeClient.cpp
            common/constants.h
            common/types.h
            common/utils.h common/pipes/ServerChannel.cpp common/pipes/ServerChannel.h common/thread/Thread.cpp common/thread/Thread.h common/thread/Event.cpp common/thread/Event.h common/thread/Mutex.cpp common/thread/Mutex.h common/thread/Event.cpp common/thread/Event.h common/pipes/PipeRequest.h common/server/RouteServer.cpp common/server/RouteServer.h common/pipes/ClientChannel.cpp common/pipes/ClientChannel.h common/pipes/RequestDecoder.cpp common/pipes/RequestDecoder.h)
target_link_libraries(RouteCommon
        PUBLIC CONAN_PKG::argparse
        PUBLIC CONAN_PKG::spdlog)
target_include_directories(RouteCommon
        PUBLIC common)

# examples
add_subdirectory(examples/basic-client)

add_executable(RouteD routed.cpp)
target_link_libraries(RouteD
        PRIVATE RouteCommon)