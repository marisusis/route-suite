#include <iostream>
#include <chrono>
#include <thread>
#include <regex>
#include "spdlog/spdlog.h"
#include "sndfile.h"
#include "argparse/argparse.hpp"
#include "portaudio.h"

struct player_data {
    SNDFILE* sndfile;
    SF_INFO* sfInfo;
    int position;
    int sampleRate;
    int bufferSize;
};

int callback(const void *input,
             void *output,
             unsigned long frameCount,
             const PaStreamCallbackTimeInfo* paTimeInfo,
             PaStreamCallbackFlags statusFlags,
             void *userData);


int main(int argc, char *argv[]) {

    // configure spdlog
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S:%e] [thread %t] %^[%l] %v%$");

    // configure argparse
    argparse::ArgumentParser program("play_audio_file");

    // option for verbose logging
    program.add_argument("-v", "--verbose")
            .help("increase output verbosity")
            .default_value(false)
            .implicit_value(true);


    // sample rate option
    program.add_argument("-s","--sample_rate")
        .help("set the sample rate")
        .default_value(44100)
        .scan<'i',int>();

    // buffer size option
    program.add_argument("-b","--buffer_size")
            .help("set the buffer size")
            .default_value(256)
            .scan<'i',int>();

    // file path option
    program.add_argument("audio_file")
    .required()
    .help("path of the audio file to play");

    // parse arguments
    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    // set verbose logging
    if (program.get<bool>("-v")) {
        spdlog::set_level(spdlog::level::debug);
        spdlog::info("verbose logging enabled.");
    }

    const int sampleRate = program.get<int>("--sample_rate");
    const int bufferSize = program.get<int>("--buffer_size");
    const std::string filePath = program.get<std::string>("audio_file");

    // log the configuration
    spdlog::info("sample rate: {}hz", sampleRate);
    spdlog::info("buffer size: {} samples", bufferSize);
    spdlog::info("audio file: {}", filePath);

    // create sndfile info
    SF_INFO sfInfo;
    sfInfo.format = 0;

    // open the file
    spdlog::debug("opening file {}...", filePath);
    SNDFILE* sndfile = sf_open(filePath.c_str(), SFM_READ, &sfInfo);

    // check sound file was created
    if (sndfile == nullptr) {
        spdlog::error("unable to open file {} ({})", filePath, sf_strerror(sndfile));
        exit(1);
    }

    // create player data
    spdlog::debug("forming player data...");
    player_data* playerData = new player_data();
    playerData->sfInfo = &sfInfo;
    playerData->sndfile = sndfile;
    playerData->sampleRate = sampleRate;
    playerData->bufferSize = bufferSize;

    // initialize PortAudio
    spdlog::info("preparing PortAudio...");
    Pa_Initialize();

    // get number of PortAudio devices
    int deviceCount = Pa_GetDeviceCount();
    spdlog::debug("found {} PortAudio devices.", deviceCount);

    // collect WSAPI devices
    std::map<std::string, int> deviceList;
    for (int i = 0; i < deviceCount; i++) {
        // get the device info
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);

        // get host api info
        const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

        spdlog::debug("[{0}] {1}/{2}", i, hostApiInfo->name, deviceInfo->name);

        // add WASAPI devices
        if (hostApiInfo->type == 3) {
            deviceList.insert(std::pair<std::string, int>(deviceInfo->name, i));
        }
    }

    spdlog::info("found {} WASAPI devices.", deviceList.size());
    std::for_each(deviceList.begin(), deviceList.end(), [](const std::pair<std::string, int>& item){
        spdlog::debug("[{0}] {1}", item.second, item.first);
    });

    auto found = std::find_if(deviceList.begin(), deviceList.end(), [](const std::pair<std::string, int>& item) {
        bool matched = std::regex_match(item.first, std::regex("(.*)(Focusrite)(.*)"));

        if (matched) spdlog::debug("found [{0}] {1}", item.second, item.first);

        return matched;
    });

    // save our device name and index
    spdlog::info("using device [{0}] {1}.", found->second, found->first);
    const std::string deviceName = found->first;
    const int device = found->second;

    // create PortAudio stream parameters
    PaStreamParameters outputParameters;
    outputParameters.device = device;
    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = paFloat32;// | paNonInterleaved;
    outputParameters.suggestedLatency = 0.2;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    // attempt to open the stream
    PaStream* stream;
    PaError error = Pa_OpenStream(
            &stream,
            nullptr, // no input; no input parameters
            &outputParameters,
            sampleRate,
            paFramesPerBufferUnspecified,
            paNoFlag,
            callback,
            playerData
            );

    // check for any errors
    if (error) {
        spdlog::error("unable to open PortAudio stream ({})", error);
        Pa_Terminate();
        exit(1);
    }

    // start the stream
    Pa_StartStream(stream);

    // wait for a bit
    auto sleepTime = std::chrono::milliseconds(1 * 10000);
    spdlog::info("waiting for {}ms...", sleepTime.count());
    std::this_thread::sleep_for(sleepTime);

    // shut down PortAudio
    spdlog::info("terminating PortAudio...");
    Pa_Terminate();

    // delete player data when done
    delete playerData;

    return 0;

}

using timep = std::chrono::high_resolution_clock::time_point;
using hrc = std::chrono::high_resolution_clock;

timep last = hrc::now();

int callback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* paTimeInfo,
             PaStreamCallbackFlags statusFlags, void* userData) {

    const timep now = hrc::now();
    const auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now - last);

    last = hrc::now();

    // get our data structure
    auto* playerData = static_cast<player_data*>(userData);

    float* out = static_cast<float*>(output);

    spdlog::debug("diff: {}, frameCount: {}, currentTime: {}, inputBufAdcTime: {}, outputBufDacTime: {}", ((double) diff.count()) / 1e6, frameCount, paTimeInfo->currentTime, paTimeInfo->inputBufferAdcTime, paTimeInfo->outputBufferDacTime);

    // how many frames to read
    int framesToRead = frameCount;

    // output cursor
    float* cursor = static_cast<float*>(output);

    // amount read
    int read = 0;

    // while we have more to read
    while (framesToRead > 0) {

        // seek to the current position
        sf_seek(playerData->sndfile, playerData->position, SEEK_SET);

        // will we go past the end of the file?
        if (framesToRead > (playerData->sfInfo->frames - playerData->position)) {
            // only read to end of the file
            read = playerData->sfInfo->frames - playerData->position;

            // loop to beginning of file
            playerData->position = 0;
        } else {

            // fill up the buffer
            read = framesToRead;

            // increment position
            playerData->position += read;

        }

        // read into output buffer
        sf_readf_float(playerData->sndfile, cursor, read);

        // move read head
        cursor += read;

        // decrement number of samples to read
        framesToRead -= read;

    }

    return paContinue;
}
