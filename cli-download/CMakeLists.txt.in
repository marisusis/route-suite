cmake_minimum_required(VERSION 2.8.2)
project(cli-download NONE)

include(ExternalProject)
ExternalProject_Add(cli
        GIT_REPOSITORY    https://github.com/daniele77/cli
        GIT_TAG           master
        SOURCE_DIR        "${CMAKE_BINARY_DIR}/cli-src"
        BINARY_DIR        "${CMAKE_BINARY_DIR}/cli-build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        TEST_COMMAND      ""
        )