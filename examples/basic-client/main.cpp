#include "pipes/ClientChannel.h"
#include <chrono>
#include <thread>
#include <client/RouteClient.h>

int main() {

    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S:%e] [thread %t] %^[%l] %v%$");

    // create client
    route::RouteClient client("BasicClient");

    client.open();

    std::chrono::seconds sleepTime = std::chrono::seconds(1);
    spdlog::info("sleeping for {} seconds...", sleepTime.count());
    std::this_thread::sleep_for(sleepTime);

    // stop channel
    client.close();

}