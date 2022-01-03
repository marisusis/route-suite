#include <iostream>
#include <thread>
#include <chrono>

#include "spdlog/spdlog.h"
#include "common/graph/graph_manager.h"

using namespace route;

int main() {

    std::cout << "Hello, World!" << std::endl;

    // configure spdlog
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S:%e] [thread %t] %^[%l] %v%$");

    // create a nodeable


    // ok
    return 0;
}