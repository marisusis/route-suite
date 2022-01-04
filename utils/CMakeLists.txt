project(utils)

add_executable(list_portaudio_devices
        list_portaudio_devices.cpp)
target_link_libraries(list_portaudio_devices
        PRIVATE spdlog::spdlog
        PRIVATE PortAudio)

add_executable(buffer_size
        buffer_size.cpp)
target_link_libraries(buffer_size
        PRIVATE spdlog::spdlog
        PRIVATE argparse::argparse)

# from https://stackoverflow.com/a/10672739/2658449
add_custom_command(TARGET list_portaudio_devices POST_BUILD  # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PortAudio_BINARY_DIR}/portaudio.dll"        # <--this is in-file
        $<TARGET_FILE_DIR:list_portaudio_devices>)           # <--this is out-file path