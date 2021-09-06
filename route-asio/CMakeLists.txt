project(RouteASIO)

add_library(RouteASIO SHARED
        combase.cpp
        RouteASIO.def RouteASIO.cpp RouteASIO.h)

target_compile_definitions(RouteASIO PRIVATE _WINDLL)

target_link_libraries(RouteASIO
        PRIVATE asio
        PRIVATE RouteCommon)

