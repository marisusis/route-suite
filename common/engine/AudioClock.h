#ifndef ROUTE_SUITE_AUDIOCLOCK_H
#define ROUTE_SUITE_AUDIOCLOCK_H

#include <types.h>
#include <thread/Thread.h>

namespace route {

    class RouteEngine;

    class AudioClock : public Runnable {

    private:
        double sampleRate{};
        double bufferSize{};
        Thread thread;
        std::chrono::high_resolution_clock::time_point lastTime;
        std::chrono::duration<double> waitTime;
        RouteEngine* engine;

    public:
        AudioClock(RouteEngine* engine);
        ~AudioClock();

        STATUS open(int sampleRate, int bufferSize);
        STATUS close();

        STATUS start();
        STATUS stop();

        STATUS init() override;

        STATUS execute() override;

    };

}

#endif //ROUTE_SUITE_AUDIOCLOCK_H
