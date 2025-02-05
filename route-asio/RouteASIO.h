#ifndef _asiosmpl_
#define _asiosmpl_

#include "asiosys.h"
#include "client/route_client.h"

// when true, will feed the left input (to host) with
// a sine wave, and the right one with a sawtooth

enum {
    kBlockFrames = 256,
    kNumInputs = 16,
    kNumOutputs = 16
};



#include "rpc.h"
#include "rpcndr.h"


#include <windows.h>
#include "ole2.h"


#include "combase.h"
#include "iasiodrv.h"

namespace route {

    class ASIODebugger;
    class ASIOClock;


    class RouteASIO : public IASIO, public CUnknown {

    friend class ASIODebugger;
    friend class ASIOClock;

    public:
        RouteASIO(LPUNKNOWN pUnk, HRESULT* phr);

        ~RouteASIO();

        DECLARE_IUNKNOWN
        //STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {      \
    //    return GetOwner()->QueryInterface(riid,ppv);            \
    //};                                                          \
    //STDMETHODIMP_(ULONG) AddRef() {                             \
    //    return GetOwner()->AddRef();                            \
    //};                                                          \
    //STDMETHODIMP_(ULONG) Release() {                            \
    //    return GetOwner()->Release();                           \
    //};

        // Factory method
        static CUnknown* CreateInstance(LPUNKNOWN pUnk, HRESULT* phr);

        // IUnknown
        virtual HRESULT STDMETHODCALLTYPE NonDelegatingQueryInterface(REFIID riid, void** ppvObject);

        double sampleRate;
        long blockFrames;

        ASIOBool init(void* sysRef);

        void getDriverName(char* name);    // max 32 bytes incl. terminating zero
        long getDriverVersion();

        void getErrorMessage(char* string);    // max 128 bytes incl.

        ASIOError start();

        ASIOError stop();

        ASIOError getChannels(long* numInputChannels, long* numOutputChannels);

        ASIOError getLatencies(long* inputLatency, long* outputLatency);

        ASIOError getBufferSize(long* minSize, long* maxSize,
                                long* preferredSize, long* granularity);

        ASIOError canSampleRate(ASIOSampleRate sampleRate);

        ASIOError getSampleRate(ASIOSampleRate* sampleRate);

        ASIOError setSampleRate(ASIOSampleRate sampleRate);

        ASIOError getClockSources(ASIOClockSource* clocks, long* numSources);

        ASIOError setClockSource(long index);

        ASIOError getSamplePosition(ASIOSamples* sPos, ASIOTimeStamp* tStamp);

        ASIOError getChannelInfo(ASIOChannelInfo* info);

        ASIOError createBuffers(ASIOBufferInfo* bufferInfos, long numChannels,
                                long bufferSize, ASIOCallbacks* callbacks);

        ASIOError disposeBuffers();

        ASIOError controlPanel();

        ASIOError future(long selector, void* opt);

        ASIOError outputReady();

        RunState getState() const;

        void bufferSwitch();

        long getMilliSeconds() { return milliSeconds; }

        route_client* routeClient;


    private:

        friend void myTimer();

        void updateState(RunState newState);

        void processInput();

        void processOutput();

        void timerOn();

        void timerOff();

        void bufferSwitchX();

        double samplePosition;
        ASIOCallbacks* callbacks;
        ASIOTime asioTime;
        ASIOTimeStamp theSystemTime;
        ASIODebugger* dbg;
        ASIOClock* clock;
        RunState state;

        float** inputBuffers;
        float** outputBuffers;

        long toggle;
        long milliSeconds;
        bool active, started;
        bool timeInfoMode, tcRead;
        char errorMessage[128];
    };

    class ASIODebugger : public Runnable {

    private:
        Thread thread;
        RouteASIO* asio;
        long count = 0;
        std::chrono::high_resolution_clock::time_point lastTime;
        double lastDiff = 0;
        double avgBufferTimeMsec = 0;
        int loopTime = 1000;

    public:

        ASIODebugger(RouteASIO* asio, int loopTime);
        ~ASIODebugger();

        void start();

        void bufferTick();

        void stop();

        STATUS init() override;

        STATUS execute() override;

    };

}
#endif

