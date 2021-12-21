#include "pipes/ClientChannel.h"
#include <chrono>
#include <thread>
#include <client/RouteClient.h>

int main() {

    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S:%e] [thread %t] %^[%l] %v%$");

    // create client
    Route::RouteClient client("BasicClient");

    client.open();

    float* testBuffer = static_cast<float *>(malloc(sizeof(float) * MAX_BUFFER_SIZE));

    for (int i = 0; i < 100; i++) {
        testBuffer[i] = i/100.0;
    }

    client.copyToBuffer(1, testBuffer, 100, false);
    client.copyToBuffer(1, testBuffer, 100, true);


    for (int j = 0; j < 10; j++) {
        // copy values of buffer 1
        client.copyFromBuffer(0, testBuffer, 256, false);
        for (int i = 0; i < 10; i++) {
            spdlog::info("value {0} is {1}", i, testBuffer[i]);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    for (int i = 0; i < 100; i++) {
        testBuffer[i] = 0;
    }

    client.copyToBuffer(1, testBuffer, 100, false);
    client.copyToBuffer(1, testBuffer, 100, true);

//    std::chrono::seconds sleepTime = std::chrono::seconds(10);
//    spdlog::info("sleeping for {} seconds...", sleepTime.count());
//    std::this_thread::sleep_for(sleepTime);

    // stop channel
    client.close();

}