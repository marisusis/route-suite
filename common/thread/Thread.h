#ifndef ROUTE_SUITE_THREAD_H
#define ROUTE_SUITE_THREAD_H

#include <windows.h>
#include <types.h>
#include <constants.h>
#include <atomic>
#include <string>
#include "utils.h"

namespace route {

    typedef DWORD (WINAPI *ThreadCallback)(void* arg);

    enum ThreadState {
        IDLE, STARTING, INITIALIZING, RUNNING
    };

    static std::string threadStateToString(ThreadState state) {
        switch (state) {
            case IDLE:
                return "IDLE";
            case STARTING:
                return "STARTING";
            case INITIALIZING:
                return "INITIALIZING";
            case RUNNING:
                return "RUNNING";
            default:
                CRT_CTX(threadStateToString, "bad ThreadState!");
                return "BAD THREAD STATE";
        }
    }

    class Runnable {
    protected:
        Runnable() = default;
        virtual ~Runnable() = default;

    public:
        virtual STATUS init() {
            return STATUS_OK;
        }

        virtual STATUS execute() = 0;
    };

    class Thread {

    private:
        HANDLE threadHandle;
        HANDLE eventHandle;
        static DWORD WINAPI ThreadHandler(void* arg);
        const char* threadName;
        bool realtime = false;

    protected:
        Runnable* runnable;
        std::atomic<ThreadState> threadState;

    public:

        Thread(Runnable* runnable, const char* thread_name, bool realtime);
        explicit Thread(Runnable* runnable, const char* thread_name);
        ~Thread();

        STATUS start();
        STATUS startSync();
        STATUS startInternal(HANDLE* thread_handle, int priority, int realtime, ThreadCallback start_routine, void* arg);

        STATUS setRealtime(bool realtime);

        STATUS kill();
        STATUS stop();

        void terminate() const;
        ThreadState getState();
        void setState(ThreadState new_state);

        bool isThread();

    };

}


#endif //ROUTE_SUITE_THREAD_H
