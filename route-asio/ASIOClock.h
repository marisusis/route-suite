//
// Created by maris on 12/21/2021.
//

#ifndef ROUTE_SUITE_ASIOCLOCK_H
#define ROUTE_SUITE_ASIOCLOCK_H


#include "thread/Thread.h"
#include "RouteASIO.h"
#include <chrono>

namespace Route {

    class RouteASIO;

    class ASIOClock : public Runnable {

    private:
        Thread thread;
        RouteASIO* driver;
        std::chrono::high_resolution_clock::time_point lastTime;
        std::chrono::microseconds dur;

    public:
        ASIOClock(RouteASIO *driver);
        ~ASIOClock();

        STATUS start();
        STATUS stop();

        STATUS init() override;

        STATUS execute() override;
    };

}


#endif //ROUTE_SUITE_ASIOCLOCK_H
