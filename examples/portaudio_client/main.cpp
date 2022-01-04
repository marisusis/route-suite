#include <iostream>
#include <chrono>
#include <thread>
#include <regex>
#include "spdlog/spdlog.h"
#include "argparse/argparse.hpp"
#include "portaudio.h"
#include "client/route_client.h"

struct player_data {
    route::route_client* client;
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

    // log the configuration
    spdlog::info("sample rate: {}hz", sampleRate);
    spdlog::info("buffer size: {} samples", bufferSize);

    // create route client
    route::route_client client("portaudio_client");

    // open the client
    if (client.open() != STATUS_OK) {
        spdlog::error("unable to open client!");
        exit(1);
    }

    // create player data
    spdlog::debug("forming player data...");
    player_data* playerData = new player_data();
    playerData->sampleRate = sampleRate;
    playerData->bufferSize = bufferSize;
    playerData->client = &client;
    playerData->position = 0;


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
        if (hostApiInfo->type == PaHostApiTypeId::paASIO) {
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
    outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
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

        // close route client
        client.close();

        exit(1);
    }

    // start the stream
    Pa_StartStream(stream);

    // wait for a bit
    auto sleepTime = std::chrono::milliseconds(1000 * 1000);
    spdlog::info("waiting for {}ms...", sleepTime.count());
    std::this_thread::sleep_for(sleepTime);

    // shut down PortAudio
    spdlog::info("terminating PortAudio...");
    Pa_Terminate();

    // close the client
    client.close();

    // delete player data when done
    delete playerData;

    return 0;

}

int callback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* paTimeInfo,
             PaStreamCallbackFlags statusFlags, void* userData) {

    // array of pointers to separate buffers
    float** out = static_cast<float**>(output);

    // each channel
    float* ch1 = out[0];
    float* ch2 = out[1];

    // get our data structure
    auto* playerData = static_cast<player_data*>(userData);

    route::route_client* client = playerData->client;

    route::buffer_info* buf1 = client->get_buffer(true, 0);
    route::buffer_info* buf2 = client->get_buffer(true, 1);

    memcpy(ch1, buf1->toggle ? buf1->buffer2 : buf1->buffer1, sizeof(float) * frameCount);
    memcpy(ch2, buf2->toggle ? buf2->buffer2 : buf2->buffer1, sizeof(float) * frameCount);

//    for (int i = 0; i < 10; i++) {
//        if (i % 2 == 0) {
//            out[i] = 0.5;
//        } else {
//            out[i] = -0.5;
//        }
//    }

    return paContinue;
}
