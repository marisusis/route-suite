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
    Route::ClientOpenResult result;
    channel.serverCall(&request, &result);

    // get our result
    spdlog::info("ref={}", result.referenceNumber);

    std::chrono::seconds sleepTime = std::chrono::seconds(4);
    spdlog::info("sleeping for {} seconds...", sleepTime.count());
    std::this_thread::sleep_for(sleepTime);

    // send something
    Route::ClientCloseRequest request2("BasicClient", _getpid());
    channel.serverSend(&request2);

    // stop channel
    channel.stop();

    // close channel
    channel.close();

}