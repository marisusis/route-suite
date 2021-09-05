#include "pipes/ClientChannel.h"
#include <chrono>
#include <thread>

int main() {

    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S:%e] [thread %t] %^[%l] %v%$");

    // create client
    Route::ClientChannel channel;

    // open channel
    channel.open(SERVER_NAME, "BasicClient");

    // start the channel
    channel.start();

    // send something
    Route::ClientOpenRequest request("BasicClient", _getpid());
    channel.serverCall(&request, nullptr);

    std::chrono::seconds sleepTime = std::chrono::seconds(1000);
    spdlog::info("sleeping for {} seconds...", sleepTime.count());
    std::this_thread::sleep_for(sleepTime);

    // stop channel
    channel.stop();

    // close channel
    channel.close();

}