project(RouteJS)

execute_process(COMMAND npm install)

execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )

execute_process(COMMAND node -p "require('node-api-headers').include_dir"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_API_HEADER_DIR
        )

message("${NODE_ADDON_API_DIR}")
message("${NODE_API_HEADER_DIR}")

add_library(RouteJS STATIC hello.cc APIClientWrap.cpp APIClientWrap.h)

target_include_directories(RouteJS
        PRIVATE node_modules/node-addon-api
        PRIVATE node_modules/node-api-headers/include)

target_link_libraries(RouteJS
        PUBLIC RouteAPI)