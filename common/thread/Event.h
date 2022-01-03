#ifndef ROUTE_SUITE_EVENT_H
#define ROUTE_SUITE_EVENT_H

#include <windows.h>
#include <types.h>
#include <thread>

namespace route {

    class Event {

    private:
        HANDLE eventHandle;

    public:
        Event();
        ~Event();

        STATUS wait();
        STATUS timedWait(std::chrono::milliseconds wait_time);

    };

}


#endif //ROUTE_SUITE_EVENT_H
