cmake_minimum_required(VERSION 2.8.2)
project(portaudio-download NONE)

include(ExternalProject)
ExternalProject_Add(portaudio
        GIT_REPOSITORY    https://github.com/PortAudio/portaudio
        GIT_TAG           master
        SOURCE_DIR        "${CMAKE_BINARY_DIR}/portaudio-src"
        BINARY_DIR        "${CMAKE_BINARY_DIR}/portaudio-build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        TEST_COMMAND      ""
        )