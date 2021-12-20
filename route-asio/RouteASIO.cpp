/*
	Steinberg Audio Stream I/O API
	(c) 1996, Steinberg Soft- und Hardware GmbH
	charlie (May 1996)

	asiosmpl.cpp
	
	sample implementation of asio. can be set to simulate input with some
	stupid oscillators.
	this driver doesn't output sound at all...
	timing is done via the extended time manager on the mac, and
	a simple thread on pc.
	you may test various configurations by changing the kNumInputs/Outputs,
	and kBlockFrames. note that when using wave generation, as i/o is not optimized
	at all, it moves quite a bit of memory, too many i/o channels may make it
	pretty slow.

	if you use this file as a template, make sure to resolve places where the
	search string !!! can be found...
*/

#include <stdio.h>
#include <string.h>
#include "RouteASIO.h"
#include "utils.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#define REG_NAME "Debug ASIO v2"
#define LOG_PATH "C:/Users/maris/CLionProjects/RouteExamples/cmake-build-debug/examples/debug-asio-driver/driver.log"
//#include "virtape.h"



// extern
void getNanoSeconds(ASIOTimeStamp* time);

// local

double AsioSamples2double(ASIOSamples* samples);

static const double twoRaisedTo32 = 4294967296.;
static const double twoRaisedTo32Reciprocal = 1. / twoRaisedTo32;

// on windows, we do the COM stuff.

#include "windows.h"
#include "mmsystem.h"

// class id. !!! NOTE: !!! you will obviously have to create your own class id!
// {12345678-D565-11d2-854F-00A0C99F5D19}
CLSID IID_ASIO_DRIVER = {0x12345678, 0xd565, 0x11d2, {0x85, 0x4f, 0x0, 0xa0, 0xc9, 0x9f, 0x5d, 0x19}};

CFactoryTemplate g_Templates[1] = {
        {L"ASIOSAMPLE", &IID_ASIO_DRIVER, Route::RouteASIO::CreateInstance}
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);



extern LONG RegisterAsioDriver(CLSID, char*, char*, char*, char*);

extern LONG UnregisterAsioDriver(CLSID, char*, char*);

HRESULT _stdcall DllRegisterServer() {
    LONG rc;
    char errstr[128];

    rc = RegisterAsioDriver(IID_ASIO_DRIVER, "RouteASIO.dll", REG_NAME, "Route ASIO", "Apartment");

    if (rc) {
        memset(errstr, 0, 128);
        sprintf(errstr, "Register Server failed ! (%d)", rc);
        MessageBox(0, (LPCTSTR) errstr, (LPCTSTR) "ASIO sample Driver", MB_OK);
        return -1;
    }

    return S_OK;
}

HRESULT _stdcall DllUnregisterServer() {
    LONG rc;
    char errstr[128];

    rc = UnregisterAsioDriver(IID_ASIO_DRIVER, "RouteASIO.dll", REG_NAME);

    if (rc) {
        memset(errstr, 0, 128);
        sprintf(errstr, "Unregister Server failed ! (%d)", rc);
        MessageBox(0, (LPCTSTR) errstr, (LPCTSTR) "ASIO Korg1212 I/O Driver", MB_OK);
        return -1;
    }

    return S_OK;
}

namespace Route {

    CUnknown* RouteASIO::CreateInstance(LPUNKNOWN pUnk, HRESULT* phr) {
        return (CUnknown*) new RouteASIO(pUnk, phr);
    };

    STDMETHODIMP RouteASIO::NonDelegatingQueryInterface(REFIID riid, void** ppv) {
        if (riid == IID_ASIO_DRIVER) {
            return GetInterface(this, ppv);
        }
        return CUnknown::NonDelegatingQueryInterface(riid, ppv);
    }

    RouteASIO::RouteASIO(LPUNKNOWN pUnk, HRESULT* phr) : CUnknown("ASIOSAMPLE", pUnk, phr) {

        // create logger
        spdlog::set_level(spdlog::level::debug);
        DBG_CTX(RouteASIO::new, "");
        LOG_CTX(RouteASIO::new, "hello!");

        // create RouteClient
        routeClient = new RouteClient("RouteASIO");

        blockFrames = 1024;
        inputLatency = blockFrames;        // typically
        outputLatency = blockFrames * 2;
        // typically blockFrames * 2; try to get 1 by offering direct buffer
        // access, and using asioPostOutput for lower latency
        samplePosition = 0;
        sampleRate = 44100.;
        milliSeconds = (long) ((double) (kBlockFrames * 1000) / sampleRate);
        active = false;
        started = false;
        timeInfoMode = false;
        tcRead = false;
        for (int i = 0; i < kNumInputs; i++) {
            inputBuffers[i] = nullptr;
            inMap[i] = 0;
        }

        for (int i = 0; i < kNumOutputs; i++) {
            outputBuffers[i] = nullptr;
            outMap[i] = 0;
        }
        callbacks = nullptr;
        activeInputs = activeOutputs = 0;
        toggle = 0;

    }


    RouteASIO::~RouteASIO() {
        DBG_CTX(RouteASIO::~, "");

        // close channel
        routeClient->close();

        // delete client
        delete routeClient;

        stop();
        outputClose();
        inputClose();
        disposeBuffers();
    }


    void RouteASIO::getDriverName(char* name) {
        strcpy(name, "Route ASIO");
    }


    long RouteASIO::getDriverVersion() {
        return 0x00000001L;
    }


    void RouteASIO::getErrorMessage(char* string) {
        strcpy(string, errorMessage);
    }


    ASIOBool RouteASIO::init(void* sysRef) {
        DBG_CTX(RouteASIO::init, "");

        // open channel
        STATUS openStatus = routeClient->open();
        DBG_CTX(RouteASIO::init, "opened with status {0}", statusToString(openStatus));

        if (openStatus != STATUS_OK) {
            // unable to open client
            CRT_CTX(RouteASIO::init, "unable to open driver with status [{0}]", statusToString(openStatus));

            // close inputs and outputs
            inputClose();
            outputClose();

            // timer off
            timerOff();

            return false;
        }

        // update block frames
        blockFrames = routeClient->getBufferSize();
        inputLatency = blockFrames;        // typically
        outputLatency = blockFrames * 2;


        sysRef = sysRef;
        if (active)
            return true;
        strcpy(errorMessage, "ASIO Driver open Failure!");
        if (inputOpen()) {
            if (outputOpen()) {
                active = true;
                return true;
            }
        }
        timerOff();        // de-activate 'hardware'

        outputClose();
        inputClose();
        return false;
    }


    ASIOError RouteASIO::start() {
        DBG_CTX(RouteASIO::start, "");

        if (callbacks) {
            started = false;
            samplePosition = 0;
            theSystemTime.lo = theSystemTime.hi = 0;
            toggle = 0;

            timerOn();            // activate 'hardware'
            started = true;

            return ASE_OK;
        }
        return ASE_NotPresent;
    }


    ASIOError RouteASIO::stop() {
        DBG_CTX(RouteASIO::stop, "");
        started = false;
        timerOff();        // de-activate 'hardware'
        return ASE_OK;
    }


    ASIOError RouteASIO::getChannels(long* numInputChannels, long* numOutputChannels) {
        *numInputChannels = kNumInputs;
        *numOutputChannels = kNumOutputs;
        return ASE_OK;
    }


    ASIOError RouteASIO::getLatencies(long* _inputLatency, long* _outputLatency) {
        *_inputLatency = inputLatency;
        *_outputLatency = outputLatency;
        return ASE_OK;
    }


    ASIOError RouteASIO::getBufferSize(long* minSize, long* maxSize,
                                       long* preferredSize, long* granularity) {
        *minSize = *maxSize = *preferredSize = blockFrames;        // allow this size only
        *granularity = 0;
        return ASE_OK;
    }


    ASIOError RouteASIO::canSampleRate(ASIOSampleRate sampleRate) {
        LOG_CTX(RouteASIO::canSampleRate, "check sample rate {}hz", sampleRate);

        if (sampleRate == routeClient->getSampleRate())        // allow these rates only
            return ASE_OK;
        return ASE_NoClock;
    }


    ASIOError RouteASIO::getSampleRate(ASIOSampleRate* sampleRate) {
        LOG_CTX(RouteASIO::getSampleRate, "get sample rate {}hz", *sampleRate);
        *sampleRate = routeClient->getSampleRate();
        return ASE_OK;
    }


    ASIOError RouteASIO::setSampleRate(ASIOSampleRate sampleRate) {
        LOG_CTX(RouteASIO::setSampleRate, "set sample rate {}hz", sampleRate);

        if (sampleRate != routeClient->getSampleRate())
            return ASE_NoClock;
        if (sampleRate != this->sampleRate) {
            this->sampleRate = sampleRate;
            asioTime.timeInfo.sampleRate = sampleRate;
            asioTime.timeInfo.flags |= kSampleRateChanged;
            milliSeconds = (long) ((double) (kBlockFrames * 1000) / this->sampleRate);
            if (callbacks && callbacks->sampleRateDidChange)
                callbacks->sampleRateDidChange(this->sampleRate);
        }
        return ASE_OK;
    }


    ASIOError RouteASIO::getClockSources(ASIOClockSource* clocks, long* numSources) {
        // internal
        clocks->index = 0;
        clocks->associatedChannel = -1;
        clocks->associatedGroup = -1;
        clocks->isCurrentSource = ASIOTrue;
        strcpy(clocks->name, "Internal");
        *numSources = 1;
        return ASE_OK;
    }


    ASIOError RouteASIO::setClockSource(long index) {
        if (!index) {
            asioTime.timeInfo.flags |= kClockSourceChanged;
            return ASE_OK;
        }
        return ASE_NotPresent;
    }


    ASIOError RouteASIO::getSamplePosition(ASIOSamples* sPos, ASIOTimeStamp* tStamp) {
//    LOG_CTX(RouteASIO::getSamplePosition, "sample position=[hi={}, lo={}], ts=[hi={}, lo={}]", sPos->hi, sPos->lo, tStamp->hi, tStamp->lo);
        tStamp->lo = theSystemTime.lo;
        tStamp->hi = theSystemTime.hi;
        if (samplePosition >= twoRaisedTo32) {
            sPos->hi = (unsigned long) (samplePosition * twoRaisedTo32Reciprocal);
            sPos->lo = (unsigned long) (samplePosition - (sPos->hi * twoRaisedTo32));
        } else {
            sPos->hi = 0;
            sPos->lo = (unsigned long) samplePosition;
        }
        return ASE_OK;
    }


    ASIOError RouteASIO::getChannelInfo(ASIOChannelInfo* info) {
//        DBG_CTX(RouteASIO::getChannelInfo, "{} channel #{} named {}", info->isInput ? "input" : "output", info->channel,
//                info->name);
        if (info->channel < 0 || (info->isInput ? info->channel >= kNumInputs : info->channel >= kNumOutputs))
            return ASE_InvalidParameter;

        info->type = ASIOSTInt16LSB;
        info->channelGroup = 0;
        info->isActive = ASIOFalse;
        long i;
        if (info->isInput) {
            for (i = 0; i < activeInputs; i++) {
                if (inMap[i] == info->channel) {
                    info->isActive = ASIOTrue;
                    break;
                }
            }
        } else {
            for (i = 0; i < activeOutputs; i++) {
                if (outMap[i] == info->channel) {
                    info->isActive = ASIOTrue;
                    break;
                }
            }
        }

        strcpy_s(info->name, sizeof(char) * 32,
                 format_string("[%d] Route %s %d", routeClient->getRef(), info->isInput ? "Source" : "Sink", info->channel + 1).c_str());

        return ASE_OK;
    }


    ASIOError RouteASIO::createBuffers(ASIOBufferInfo* bufferInfos, long numChannels,
                                       long bufferSize, ASIOCallbacks* callbacks) {
        DBG_CTX(RouteASIO::createBuffers, "numChannels={}, bufferSize={}", numChannels, bufferSize);

        ASIOBufferInfo* info = bufferInfos;
        long i;
        bool notEnoughMem = false;

        activeInputs = 0;
        activeOutputs = 0;
        blockFrames = routeClient->getBufferSize(); // NEW: get buffer size from client; bufferSize;
        for (i = 0; i < numChannels; i++, info++) {
            if (info->isInput) {
                if (info->channelNum < 0 || info->channelNum >= kNumInputs)
                    goto error;
                inMap[activeInputs] = info->channelNum;
                inputBuffers[activeInputs] = new float[blockFrames * 2];    // double buffer
                if (inputBuffers[activeInputs]) {
                    info->buffers[0] = inputBuffers[activeInputs];
                    info->buffers[1] = inputBuffers[activeInputs] + blockFrames;
                } else {
                    info->buffers[0] = info->buffers[1] = 0;
                    notEnoughMem = true;
                }
                activeInputs++;
                if (activeInputs > kNumInputs) {
                    error:
                    disposeBuffers();
                    return ASE_InvalidParameter;
                }
            } else    // output
            {
                if (info->channelNum < 0 || info->channelNum >= kNumOutputs)
                    goto error;
                outMap[activeOutputs] = info->channelNum;
                outputBuffers[activeOutputs] = new float[blockFrames * 2];    // double buffer
                if (outputBuffers[activeOutputs]) {
                    info->buffers[0] = outputBuffers[activeOutputs];
                    info->buffers[1] = outputBuffers[activeOutputs] + blockFrames;
                } else {
                    info->buffers[0] = info->buffers[1] = 0;
                    notEnoughMem = true;
                }
                activeOutputs++;
                if (activeOutputs > kNumOutputs) {
                    activeOutputs--;
                    disposeBuffers();
                    return ASE_InvalidParameter;
                }
            }
        }
        if (notEnoughMem) {
            disposeBuffers();
            return ASE_NoMemory;
        }

        this->callbacks = callbacks;
        if (callbacks->asioMessage(kAsioSupportsTimeInfo, 0, 0, 0)) {
            timeInfoMode = true;
            asioTime.timeInfo.speed = 1.;
            asioTime.timeInfo.systemTime.hi = asioTime.timeInfo.systemTime.lo = 0;
            asioTime.timeInfo.samplePosition.hi = asioTime.timeInfo.samplePosition.lo = 0;
            asioTime.timeInfo.sampleRate = sampleRate;
            asioTime.timeInfo.flags = kSystemTimeValid | kSamplePositionValid | kSampleRateValid;

            asioTime.timeCode.speed = 1.;
            asioTime.timeCode.timeCodeSamples.lo = asioTime.timeCode.timeCodeSamples.hi = 0;
            asioTime.timeCode.flags = kTcValid | kTcRunning;
        } else
            timeInfoMode = false;
        return ASE_OK;
    }


    ASIOError RouteASIO::disposeBuffers() {
        DBG_CTX(RouteASIO::disposeBuffers, "");
        long i;

        callbacks = 0;
        stop();
        for (i = 0; i < activeInputs; i++)
            delete inputBuffers[i];
        activeInputs = 0;
        for (i = 0; i < activeOutputs; i++)
            delete outputBuffers[i];
        activeOutputs = 0;
        return ASE_OK;
    }


    ASIOError RouteASIO::controlPanel() {
        DBG_CTX(RouteASIO::controlPanel, "opening control panel...");
        routeClient->openConfig();
        return ASE_NotPresent;
    }


    ASIOError RouteASIO::future(long selector, void* opt)    // !!! check properties
    {
        DBG_CTX(RouteASIO::future, "FUTURE!!!");

        ASIOTransportParameters* tp = (ASIOTransportParameters*) opt;
        switch (selector) {
            case kAsioEnableTimeCodeRead:
                DBG_CTX(RouteASIO::future, "kAsioEnableTimeCodeRead");
                tcRead = true;
                return ASE_SUCCESS;
            case kAsioDisableTimeCodeRead:
                DBG_CTX(RouteASIO::future, "kAsioDisableTimeCodeRead");

                tcRead = false;
                return ASE_SUCCESS;
            case kAsioSetInputMonitor:
                DBG_CTX(RouteASIO::future, "kAsioSetInputMonitor");

                return ASE_SUCCESS;    // for testing!!!
            case kAsioCanInputMonitor:
                DBG_CTX(RouteASIO::future, "kAsioCanInputMonitor");

                return ASE_SUCCESS;    // for testing!!!
            case kAsioCanTimeInfo:
                DBG_CTX(RouteASIO::future, "kAsioCanTimeInfo");

                return ASE_SUCCESS;
            case kAsioCanTimeCode:
                DBG_CTX(RouteASIO::future, "kAsioCanTimeCode");

                return ASE_SUCCESS;
        }
        return ASE_NotPresent;
    }


// private methods



// input



    bool RouteASIO::inputOpen() {

        return true;
    }


    void RouteASIO::inputClose() {

    }


    void RouteASIO::processInput() {
        // iterate through all input buffers
        float* in = 0;
        float* out = 0;
        for (long i = 0; i < activeInputs; i++) {
            // pointer to beginning of buffer number i
            in = inputBuffers[i];
            out = outputBuffers[i];
            if (in) {
                if (toggle) {// read first or second half of double-buffer
                    in += blockFrames;
                    out += blockFrames;
                }

                memcpy(in, out, (unsigned long) (blockFrames*2));
            }
        }
    }


// output



    bool RouteASIO::outputOpen() {
        return true;
    }


    void RouteASIO::outputClose() {
    }


    void RouteASIO::processOutput() {
    }


    void RouteASIO::bufferSwitch() {
        if (started && callbacks) {
            getNanoSeconds(&theSystemTime);            // latch system time
            processInput();
            processOutput();
            samplePosition += blockFrames;
            if (timeInfoMode)
                bufferSwitchX();
            else
                callbacks->bufferSwitch(toggle, ASIOFalse);
            toggle = toggle ? 0 : 1;
        }
    }


// asio2 buffer switch
    void RouteASIO::bufferSwitchX() {
        getSamplePosition(&asioTime.timeInfo.samplePosition, &asioTime.timeInfo.systemTime);
        long offset = toggle ? blockFrames : 0;
        if (tcRead) {    // Create a fake time code, which is 10 minutes ahead of the card's sample position
            // Please note that for simplicity here time code will wrap after 32 bit are reached
            asioTime.timeCode.timeCodeSamples.lo = asioTime.timeInfo.samplePosition.lo + 600.0 * sampleRate;
            asioTime.timeCode.timeCodeSamples.hi = 0;
        }
        callbacks->bufferSwitchTimeInfo(&asioTime, toggle, ASIOFalse);
        asioTime.timeInfo.flags &= ~(kSampleRateChanged | kClockSourceChanged);
    }


    ASIOError RouteASIO::outputReady() {
        return ASE_NotPresent;
    }

}


double AsioSamples2double(ASIOSamples* samples) {
    double a = (double) (samples->lo);
    if (samples->hi)
        a += (double) (samples->hi) * twoRaisedTo32;
    return a;
}

