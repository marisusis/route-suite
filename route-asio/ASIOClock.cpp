//
// Created by maris on 12/21/2021.
//

#include "ASIOClock.h"
#include "RouteASIO.h"
#include <chrono>
#include <math.h>
#include <thread>

using namespace std::chrono_literals;

Route::ASIOClock::ASIOClock(RouteASIO *driver) : thread(this, "ASIOClock"), driver(driver) {
    DBG_CTX(ASIOClock::new, "");

}

Route::ASIOClock::~ASIOClock() {
    DBG_CTX(ASIOClock::~, "");
}

STATUS Route::ASIOClock::init() {

    const double sampleRate = driver->routeClient->getSampleRate();
    const double bufferSize = driver->routeClient->getBufferSize();

    const int factor = (bufferSize * 1e6/sampleRate);

    // calculate wait time
    dur = 1us * factor;

    LOG_CTX(ASIOClock::init, "sleep time {0}us", dur.count());

    return Runnable::init();
}

STATUS Route::ASIOClock::execute() {

    // wait until enough time has passed
    std::this_thread::sleep_until(lastTime + dur);

    driver->bufferSwitch();

    lastTime = std::chrono::high_resolution_clock::now();

    return STATUS_OK;
}

STATUS Route::ASIOClock::stop() {
    LOG_CTX(ASIOClock::stop, "stopping clock...");

    // stop the clock thread
    thread.stop();

    return STATUS_OK;
}

STATUS Route::ASIOClock::start() {
    LOG_CTX(ASIOClock::start, "starting clock...");

    // get the last time
    lastTime = std::chrono::high_resolution_clock::now();

    // start the thread
    thread.start();

    return STATUS_OK;
}
