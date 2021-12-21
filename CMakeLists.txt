cmake_minimum_required(VERSION 3.19)
project(route_suite)

add_compile_definitions(_WINDLL)


function(download_file url filename hash_type hash)

    if(NOT EXISTS ${filename})
        file(DOWNLOAD ${url} ${filename}
                TIMEOUT 60  # seconds
                EXPECTED_HASH ${hash_type}=${hash})
    endif()

endfunction(download_file)

# using at least C++17
set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeModules")

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
            common/utils.h common/pipes/ServerChannel.cpp common/pipes/ServerChannel.h common/thread/Thread.cpp common/thread/Thread.h common/thread/Event.cpp common/thread/Event.h common/thread/Mutex.cpp common/thread/Mutex.h common/thread/Event.cpp common/thread/Event.h common/pipes/PipeRequest.h common/server/RouteServer.cpp common/server/RouteServer.h common/pipes/ClientChannel.cpp common/pipes/ClientChannel.h common/pipes/RequestDecoder.cpp common/pipes/RequestDecoder.h common/client/RouteClient.cpp common/client/RouteClient.h common/utils.cpp common/server/client/ClientManager.cpp common/server/client/ClientManager.h common/server/client/Client.cpp common/server/client/Client.h common/pipes/NotifyClientChannel.cpp common/pipes/NotifyClientChannel.h common/shared/SharedStructures.h common/server/BufferManager.cpp common/server/BufferManager.h)
target_link_libraries(RouteCommon
        PUBLIC CONAN_PKG::argparse
        PUBLIC CONAN_PKG::spdlog
        PUBLIC CONAN_PKG::boost)
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
        PRIVATE RouteCommon)