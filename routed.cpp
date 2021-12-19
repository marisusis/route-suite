#include <iostream>
#include "spdlog/spdlog.h"
#include "common/server/RouteServer.h"
#include <chrono>
#include <thread>

int main() {
    std::cout << "Hello, World!" << std::endl;

    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S:%e] [thread %t] %^[%l] %v%$");

    // create route server instance
    Route::RouteServer server;

    // open server
    server.open();

    // start the server
    server.start();

    std::chrono::seconds sleepTime = std::chrono::seconds(1000);
    spdlog::info("sleeping for {} seconds...", sleepTime.count());
    std::this_thread::sleep_for(sleepTime);

    // stop the server
    server.stop();

    // close server
    server.close();


    return 0;
}
