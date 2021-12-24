project(RouteASIO)

add_library(RouteASIO SHARED
        combase.cpp
        combase.h
        dllentry.cpp
        register.cpp
        wintimer.cpp
        RouteASIO.def
        RouteASIO.cpp
        RouteASIO.h ASIOClock.cpp ASIOClock.h)

target_compile_definitions(RouteASIO PRIVATE _WINDLL)

target_link_libraries(RouteASIO
        PRIVATE spdlog::spdlog
        PRIVATE asio
        PRIVATE RouteCommon
        PRIVATE winmm)



