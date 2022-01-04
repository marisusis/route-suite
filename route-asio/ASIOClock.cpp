//
// Created by maris on 12/21/2021.
//

#include "ASIOClock.h"
#include "RouteASIO.h"
#include <chrono>
#include <ratio>
#include <math.h>
#include <thread>

const double twoRaisedTo32 = 4294967296.;

using namespace std::chrono_literals;

route::ASIOClock::ASIOClock(RouteASIO *driver) : thread(this, "ASIOClock", true), driver(driver) {
    DBG_CTX(ASIOClock::new, "");

}

route::ASIOClock::~ASIOClock() {
    DBG_CTX(ASIOClock::~, "");
}

STATUS route::ASIOClock::init() {

    const double sampleRate = driver->routeClient->get_sample_rate();
    const double bufferSize = driver->routeClient->get_buffer_size();

    waitTime = std::chrono::duration<double>(bufferSize/sampleRate);

    LOG_CTX(ASIOClock::init, "sleep time {0}s for bufferSize={1} / sampleRate={2}", waitTime.count(), bufferSize, sampleRate);

    return Runnable::init();
}

STATUS route::ASIOClock::execute() {

//    auto until = lastTime + waitTime;

//    if (waitTime < (std::chrono::high_resolution_clock::now() - lastTime)) {
//        return STATUS_OK;
//    }

    // wait until enough time has passed
//    std::this_thread::sleep_until(lastTime + waitTime);

        if (thread.getState() != RUNNING) return STATUS_BREAK;
//    while (thread.getState() == RUNNING) {

        if ((std::chrono::high_resolution_clock::now() - lastTime) < waitTime) {
            return STATUS_OK;
        }

        // update last time
        lastTime = std::chrono::high_resolution_clock::now();


        driver->bufferSwitch();

//    }

    return STATUS_OK;
}

STATUS route::ASIOClock::stop() {
    LOG_CTX(ASIOClock::stop, "stopping clock...");

    // stop the clock thread
    thread.stop();

    return STATUS_OK;
}

STATUS route::ASIOClock::start() {
    LOG_CTX(ASIOClock::start, "starting clock...");

    // get the last time
    lastTime = std::chrono::high_resolution_clock::now();

    // start the thread
    thread.start();

    return STATUS_OK;
}

void route::ASIOClock::latchTime(ASIOTimeStamp* timestamp) {

    // get current time in nanoseconds
    auto currentTime = std::chrono::high_resolution_clock::now();
    double nanoTime = std::chrono::duration<double, std::ratio<1, 1000000000>>(currentTime.time_since_epoch()).count();

    // update the timestamp
    timestamp->hi = (unsigned long)(nanoTime / twoRaisedTo32);
    timestamp->lo = (unsigned long)(nanoTime - (timestamp->hi * twoRaisedTo32));

}
