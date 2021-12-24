project(BasicClient)

add_executable(BasicClient main.cpp
        ${IMGUI_INCLUDE}/imgui_impl_dx11.cpp
        ${IMGUI_INCLUDE}/imgui_impl_win32.cpp)
target_link_libraries(BasicClient
        PRIVATE RouteCommon
        PRIVATE imgui::imgui
        PRIVATE d3d11.lib)
target_include_directories(BasicClient
        PRIVATE ${COMMON_INCLUDE}
        PRIVATE ${IMGUI_INCLUDE})