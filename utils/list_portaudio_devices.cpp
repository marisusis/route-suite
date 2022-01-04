
#include <map>
#include <set>
#include "spdlog/spdlog.h"
#include "portaudio.h"
#include "pa_win_wasapi.h"

int main() {

    // configure spdlog
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S:%e] [thread %t] %^[%l] %v%$");


    // initialize PortAudio
    spdlog::info("preparing PortAudio...");
    Pa_Initialize();

    // get number of PortAudio devices
    int deviceCount = Pa_GetDeviceCount();
    spdlog::debug("found {} PortAudio devices.", deviceCount);

    std::set<int> wasapiDevices;
    std::set<int> ksDevices;
    std::set<int> mmeDevices;

    // collect WSAPI devices
    for (int i = 0; i < deviceCount; i++) {
        // get the device info
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);

        // get host api info
        const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

        switch (hostApiInfo->type) {
            case paWASAPI:
                wasapiDevices.insert(i);
                break;
            case paMME:
                mmeDevices.insert(i);
                break;
            case paWDMKS:
                ksDevices.insert(i);
                break;
            default:
//                spdlog::warn("no support for {} yet", hostApiInfo->name);
                break;
        }



        spdlog::info("[{0}] {1} | {2} I/O channels {3}/{4}", i, hostApiInfo->name, deviceInfo->name,
                      deviceInfo->maxInputChannels, deviceInfo->maxOutputChannels);
    }

    /*spdlog::info("[WASAPI Devices]");

    std::for_each(wasapiDevices.begin(), wasapiDevices.end(), [](const int& i) {
        // get the device info
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);

        // get host api info
        const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

        spdlog::info("[{}] {} [I/O] {}/{}", i, deviceInfo->name,
                     deviceInfo->maxInputChannels, deviceInfo->maxOutputChannels);
    });*/

    // shut down PortAudio
    spdlog::info("terminating PortAudio...");
    Pa_Terminate();

    return 0;
}