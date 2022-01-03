cmake_minimum_required(VERSION 3.19)
project(route_suite)

add_compile_definitions(_WINDLL)

configure_file(cli-download/CMakeLists.txt.in cli-download/CMakeLists.txt)

execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/cli-download"
        )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/cli-download"
        )

add_subdirectory("${CMAKE_BINARY_DIR}/cli-src")

function(download_file url filename hash_type hash)

    if(NOT EXISTS ${filename})
        file(DOWNLOAD ${url} ${filename}
                TIMEOUT 60  # seconds
                EXPECTED_HASH ${hash_type}=${hash})
    endif()

endfunction(download_file)

# using at least C++17
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/conanstuff")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/conanstuff")


if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/asiosdk.zip)
    file(DOWNLOAD https://download.steinberg.net/sdk_downloads/asiosdk_2.3.3_2019-06-14.zip ${CMAKE_CURRENT_SOURCE_DIR}/asiosdk.zip
            TIMEOUT 60  # seconds
            EXPECTED_HASH SHA1=33C44F0C919A7218413EA81E06A66BB8B47BE292)

    # extract
    file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_SOURCE_DIR}/asiosdk.zip DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/asiosdk)

endif()

# ASIO
find_package(ASIO REQUIRED)

# conan.io
#include(conanbuildinfo_multi.cmake)
#conan_basic_setup(TARGETS)

set(COMMON_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/common")
set(IMGUI_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/imgui")


#find_package(DirectX11)
find_package(imgui CONFIG)
find_package(argparse CONFIG)
find_package(spdlog CONFIG)
find_package(boost CONFIG)

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
            common/utils.h 
            common/pipes/ServerChannel.cpp 
            common/pipes/ServerChannel.h
            common/thread/Thread.cpp
            common/thread/Thread.h
            common/thread/Event.cpp
            common/thread/Event.h
            common/thread/Mutex.cpp
            common/thread/Mutex.h
            common/thread/Event.cpp
            common/thread/Event.h
            common/pipes/PipeRequest.h
        common/server/route_server.cpp
        common/server/route_server.h
            common/pipes/ClientChannel.cpp
            common/pipes/ClientChannel.h
            common/pipes/RequestDecoder.cpp
            common/pipes/RequestDecoder.h
            common/client/RouteClient.cpp
            common/client/RouteClient.h
            common/utils.cpp
            common/server/client/ClientManager.cpp
            common/server/client/ClientManager.h
            common/server/client/Client.cpp
            common/server/client/Client.h
            common/pipes/NotifyClientChannel.cpp
            common/pipes/NotifyClientChannel.h
            common/shared/SharedStructures.h
            common/server/BufferManager.cpp
            common/server/BufferManager.h
            common/engine/RouteEngine.cpp
            common/engine/RouteEngine.h
            common/engine/AudioClock.cpp
            common/engine/AudioClock.h
        common/graph/graph_manager.cpp
        common/graph/graph_manager.h
            common/graph/port.cpp
            common/graph/port.h)
target_link_libraries(RouteCommon
        PUBLIC argparse::argparse
        PUBLIC spdlog::spdlog
        PUBLIC Boost::boost)
target_include_directories(RouteCommon
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

# examples
add_subdirectory(examples/basic-client)

# ASIO driver
add_subdirectory(route-asio)
add_subdirectory(bindings/node)
add_subdirectory(api)

add_executable(RouteD routed.cpp)
target_link_libraries(RouteD
        PRIVATE RouteCommon
        PRIVATE cli)

add_executable(TestGraphs test_graphs.cpp)
target_link_libraries(TestGraphs
        PRIVATE RouteCommon
        PRIVATE cli)