project(play_audio_file)

add_executable(play_audio_file main.cpp)

target_link_libraries(play_audio_file
        PRIVATE spdlog::spdlog
        PRIVATE argparse::argparse
        PRIVATE SndFile::sndfile
        PRIVATE PortAudio)
# from https://stackoverflow.com/a/10672739/2658449
add_custom_command(TARGET play_audio_file POST_BUILD        # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PortAudio_BINARY_DIR}/portaudio.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:play_audio_file>)                 # <--this is out-file path