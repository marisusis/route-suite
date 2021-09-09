project(RouteAPI)

add_library(RouteAPI SHARED
        APITypes.h
        RouteAPI.cpp
        RouteAPI.h
        RouteAPI.def)

target_include_directories(RouteAPI
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(RouteAPI
        PRIVATE RouteCommon)

add_custom_command(TARGET RouteAPI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:RouteAPI> ${CMAKE_CURRENT_SOURCE_DIR}/../bindings/node/RouteAPI.dll
        )
