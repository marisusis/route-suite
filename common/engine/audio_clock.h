#ifndef ROUTE_SUITE_AUDIO_CLOCK_H
#define ROUTE_SUITE_AUDIO_CLOCK_H

#include <types.h>
#include <thread/Thread.h>

namespace route {

    class route_engine;

    class audio_clock : public Runnable {

    private:
        double sampleRate{};
        double bufferSize{};
        Thread thread;
        std::chrono::high_resolution_clock::time_point lastTime;
        std::chrono::duration<double> waitTime;
        route_engine* engine;

    public:
        audio_clock(route_engine* engine);
        ~audio_clock();

        STATUS open(int sampleRate, int bufferSize);
        STATUS close();

        STATUS start();
        STATUS stop();

        STATUS init() override;

        STATUS execute() override;

    };

}

#endif //ROUTE_SUITE_AUDIO_CLOCK_H
