project(BasicClient)

add_executable(BasicClient main.cpp)
target_link_libraries(BasicClient
        PRIVATE RouteCommon)
target_include_directories(BasicClient
        PRIVATE ${COMMON_INCLUDE})