project(portaudio_client)

add_executable(portaudio_client main.cpp)

target_link_libraries(portaudio_client
        PRIVATE RouteCommon
        PRIVATE spdlog::spdlog
        PRIVATE argparse::argparse
        PRIVATE PortAudio)

#target_include_directories(portaudio_client
#        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common)

# from https://stackoverflow.com/a/10672739/2658449
add_custom_command(TARGET portaudio_client POST_BUILD  # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PortAudio_BINARY_DIR}/portaudio.dll"        # <--this is in-file
        $<TARGET_FILE_DIR:portaudio_client>)           # <--this is out-file path