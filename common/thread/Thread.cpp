#include "Thread.h"
#include "utils.h"

namespace Route {

    DWORD WINAPI Thread::ThreadHandler(void* arg) {

        // get thread object
        Thread* theThread = static_cast<Thread*>(arg);
        Runnable* runnable = theThread->runnable;

        // initializing
        theThread->setState(INITIALIZING);

        // call initialization function
        STATUS initStatus = runnable->init();

        if (initStatus != STATUS_OK) {
            ERR_CTX(Thread::ThreadHandler, "unable to create thread [{}], exiting with status {}!", theThread->threadName, initStatus);
            return 1;
        }

        // thread is now running
        theThread->setState(RUNNING);

        while (true) {

            // check for signal
            if (WaitForSingleObject(theThread->eventHandle, 0) == WAIT_OBJECT_0) {
                // we exit
                DBG_CTX(Thread::ThreadHandler, "exiting thread [{}]...", theThread->threadName);
                theThread->setState(IDLE);
                break;
            }

            // TODO do something with status?
            // run the function
            if (runnable->execute() != STATUS_OK) {
                return 0;
            }
        }

        return 0;
    }

    Thread::Thread(Runnable* runnable, const char* thread_name) : runnable(runnable), threadName(thread_name) {
        // thread is idle
        threadState.store(IDLE);

        // create the event
        eventHandle = CreateEvent(
                NULL, // no security attributes
                FALSE, // event resets automatically
                FALSE, // initial state unsignalled
                NULL
                );

        if (eventHandle == NULL) {
            CRT_CTX(Thread::new, "unable to create event handle for [{}]!", threadName);
        }

        // no thread handle yet
        threadHandle = INVALID_HANDLE_VALUE;


    }

    Thread::~Thread() {
        // close thread and event handle
        DBG_CTX(Thread::~, "closing event handle for thread [{}]!", threadName);
        CloseHandle(eventHandle);

        DBG_CTX(Thread::~, "closing thread handle for thread [{}]!", threadName);
        CloseHandle(threadHandle);
    }

    STATUS Thread::start() {
        DBG_CTX(Thread::start, "starting thread [{}]...", threadName);

        // thread is starting
        threadState.store(STARTING);

        // start thread
        STATUS startStatus = startInternal(&threadHandle, 0, 0, ThreadHandler, this);
        if (startStatus == STATUS_OK) {
            threadState.store(IDLE);
            return STATUS_OK;
        } else {
            return startStatus;
        }
    }

    STATUS Thread::startSync() {
        WRN_CTX(Thread::startSync, "not implemented!");

        // TODO impl
        return STATUS_ERROR;
    }

    STATUS Thread::startInternal(HANDLE* thread_handle, int priority, int realtime, ThreadCallback start_routine, void* arg) {
        // ID for the thread
        DWORD threadID;

        // create the thread
        *thread_handle = CreateThread(
                nullptr, // TODO no lp thread attributes
                0,
                start_routine,
                arg,
                0,
                &threadID
        );

        // check thread was created
        if (*thread_handle == nullptr) {
            CRT_CTX(Thread::startInternal, "unable to create thread [{}]!", threadName);
            return STATUS_NO_THREAD;
        }

        if (realtime) {
            // set time priority to critical
            DBG_CTX(Thread::startInternal, "setting thread [{}] to realtime priority...", threadName);

            if (!SetThreadPriority(*thread_handle, THREAD_PRIORITY_TIME_CRITICAL)) {
                WRN_CTX(Thread::startInternal, "cannot set thread [{}] to realtime!", threadName);
                return STATUS_OK;
            }

            return STATUS_OK;
        } else {
            DBG_CTX(Thread::startInternal, "created non-realtime thread.");
        }

        return STATUS_OK;
    }

    STATUS Thread::kill() {
        LOG_CTX(Thread::kill, "killing thread [{}]...", threadName);

        // make sure thread exists
        if (threadHandle != nullptr) {

            // terminate the thread
            TerminateThread(threadHandle, 0);

            // wait for thread to terminate
            WaitForSingleObject(threadHandle, INFINITE);

            // close thread handle
            CloseHandle(threadHandle);

            // handle is null now
            threadHandle = nullptr;

            // thread is idle
            threadState.store(IDLE);

        }

        return STATUS_ERROR;
    }

    STATUS Thread::stop() {
        DBG_CTX(Thread::stop, "stopping thread [{}]...", threadName);

        // check if thread exists
        if (threadHandle != nullptr) {

            // signal thread to stop
            SetEvent(eventHandle);

            // wait for thread object
            WaitForSingleObject(threadHandle, 0);

            // close thread handle
            CloseHandle(threadHandle);

            // thread handle is null
            threadHandle = nullptr;

            DBG_CTX(Thread::stop, "stopped thread [{}].", threadName);
            return STATUS_OK;
        }

        WRN_CTX(Thread::stop, "thread [{}] doesn't exist!", threadName);
        return STATUS_ERROR;
    }

    void Thread::terminate() const {

    }

    ThreadState Thread::getState() {
        return threadState.load();
    }

    void Thread::setState(ThreadState new_state) {
        DBG_CTX(Thread::setState, "attempting to change thread [{}] state {} -> {}...", threadName,
                threadStateToString(threadState.load()),
                threadStateToString(new_state));
        threadState.store(new_state);
        DBG_CTX(Thread::setState, "thread [{}] state now {}.", threadName, threadStateToString(new_state));
    }

    bool Thread::isThread() {
        return GetCurrentThread() == threadHandle;
    }

}
