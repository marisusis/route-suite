/*
	Steinberg Audio Stream I/O API
	(c) 1999, Steinberg Soft- und Hardware GmbH

	asiosmpl.h
	
	test implementation of asio
*/

#ifndef _asiosmpl_
#define _asiosmpl_

#include "asiosys.h"
#include "client/RouteClient.h"

#define TESTWAVES 1
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

namespace Route {

    class RouteASIO : public IASIO, public CUnknown {
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

        void bufferSwitch();

        long getMilliSeconds() { return milliSeconds; }

    private:
        RouteClient* routeClient;

        friend void myTimer();

        bool inputOpen();

#if TESTWAVES

        void makeSine(short* wave);

        void makeSaw(short* wave);

#endif

        void inputClose();

        void processInput();

        bool outputOpen();

        void outputClose();

        void processOutput();

        void timerOn();

        void timerOff();

        void bufferSwitchX();

        double samplePosition;
        double sampleRate;
        ASIOCallbacks* callbacks;
        ASIOTime asioTime;
        ASIOTimeStamp theSystemTime;
        float* inputBuffers[kNumInputs * 2];
        float* outputBuffers[kNumOutputs * 2];

        long inMap[kNumInputs];
        long outMap[kNumOutputs];
        long blockFrames;
        long inputLatency;
        long outputLatency;
        long activeInputs;
        long activeOutputs;
        long toggle;
        long milliSeconds;
        bool active, started;
        bool timeInfoMode, tcRead;
        char errorMessage[128];

        void makeSine2(short* wave, float f2);
    };

}
#endif

