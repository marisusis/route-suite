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

#include <cstdio>
#include <string>
#include "RouteASIO.h"
#include "utils.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "ASIOClock.h"

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
        {L"ASIOSAMPLE", &IID_ASIO_DRIVER, route::RouteASIO::CreateInstance}
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

extern LONG RegisterAsioDriver(CLSID clsid, const char *szdllname, const char *szregname, const char *szasiodesc, const char *szthreadmodel);

extern LONG UnregisterAsioDriver(CLSID clsid, const char *szdllname, const char *szregname);

HRESULT _stdcall DllRegisterServer() {
    LONG rc;
    char errstr[128];

    rc = RegisterAsioDriver(IID_ASIO_DRIVER, "RouteASIO.dll", REG_NAME, "route ASIO", "Apartment");

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

namespace route {

    static const char* fuck_you = "ASIOSAMPLE";

    static char* generate_shit() {
        char* str = reinterpret_cast<char*>(malloc(sizeof(char) * 10));
        memcpy(str, fuck_you, sizeof(char) * 10);
        return str;
    }


    CUnknown* RouteASIO::CreateInstance(LPUNKNOWN pUnk, HRESULT* phr) {
        return (CUnknown*) new RouteASIO(pUnk, phr);
    };

    STDMETHODIMP RouteASIO::NonDelegatingQueryInterface(REFIID riid, void** ppv) {
        if (riid == IID_ASIO_DRIVER) {
            return GetInterface(this, ppv);
        }
        return CUnknown::NonDelegatingQueryInterface(riid, ppv);
    }

    RouteASIO::RouteASIO(LPUNKNOWN pUnk, HRESULT* phr) : CUnknown(generate_shit(), pUnk, phr) {
        // configure logger
        spdlog::set_level(spdlog::level::debug);
        spdlog::set_pattern("[%H:%M:%S:%e] [thread %t] %^[%l] %v%$");

        LOG_CTX(RouteASIO::new, "Hello, world!");

        // create RouteClient
        routeClient = new route_client("RouteASIO");

        // create ASIO clock
        clock = new ASIOClock(this);

        blockFrames = 1024;
        // typically blockFrames * 2; try to get 1 by offering direct buffer
        // access, and using asioPostOutput for lower latency
        samplePosition = 0;
        sampleRate = 44100.;
        milliSeconds = (long) ((double) (kBlockFrames * 1000) / sampleRate);
        active = false;
        started = false;
        timeInfoMode = false;
        tcRead = false;

        callbacks = nullptr;
        toggle = 0;

        // create debugger
        dbg = new ASIODebugger(this,  3000);

    }

    RouteASIO::~RouteASIO() {
        DBG_CTX(RouteASIO::~, "");

        // close channel
        routeClient->close();

        // delete client
        delete routeClient;

        stop();
        disposeBuffers();
    }

    void RouteASIO::getDriverName(char* name) {
        strcpy(name, "route ASIO");
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

            // timer off
            timerOff();

            return false;
        }

        // update block frames
        blockFrames = routeClient->get_buffer_size();

        // allocate memory for buffers
        inputBuffers = new float*[routeClient->get_channel_count()]; //static_cast<float *>(malloc(sizeof(float) * MAX_BUFFER_SIZE * 2));
        outputBuffers = new float*[routeClient->get_channel_count()]; //static_cast<float *>(malloc(sizeof(float) * MAX_BUFFER_SIZE * 2));

        // create empty buffers
        for (int i = 0; i < routeClient->get_channel_count(); i++) {
            inputBuffers[i] = nullptr;
            outputBuffers[i] = nullptr;
        }

        sysRef = sysRef;

        if (routeClient->getState() == ClientStatus::OPEN) return true;

        timerOff();
        // de-activate 'hardware'

        return false;
    }


    ASIOError RouteASIO::start() {
        DBG_CTX(RouteASIO::start, "");

        updateState(RunState::STARTING);

        // start debugger

        if (callbacks) {
            started = false;
            samplePosition = 0;
            theSystemTime.lo = theSystemTime.hi = 0;
            toggle = 0;

            // start clock
            clock->start();

            started = true;

            updateState(RunState::RUNNING);

            return ASE_OK;
        }

        updateState(RunState::IDLE);
        return ASE_NotPresent;
    }


    ASIOError RouteASIO::stop() {
        DBG_CTX(RouteASIO::stop, "");

        updateState(RunState::STOPPING);

        // stop debugger
        dbg->stop();

        started = false;

        // stop clock
        clock->stop();

        updateState(RunState::IDLE);

        return ASE_OK;
    }


    ASIOError RouteASIO::getChannels(long* numInputChannels, long* numOutputChannels) {
        *numInputChannels = routeClient->get_channel_count();
        *numOutputChannels = routeClient->get_channel_count();
        return ASE_OK;
    }


    ASIOError RouteASIO::getLatencies(long* _inputLatency, long* _outputLatency) {
        *_inputLatency = routeClient->get_input_latency();
        *_outputLatency = routeClient->get_output_latency();
        return ASE_OK;
    }


    ASIOError RouteASIO::getBufferSize(long* minSize, long* maxSize,
                                       long* preferredSize, long* granularity) {
        *minSize = *maxSize = *preferredSize = routeClient->get_buffer_size();        // allow this size only
        *granularity = 0;
        return ASE_OK;
    }


    ASIOError RouteASIO::canSampleRate(ASIOSampleRate sampleRate) {
        LOG_CTX(RouteASIO::canSampleRate, "check sample rate {}hz", sampleRate);

        if (sampleRate == routeClient->get_sample_rate())        // allow these rates only
            return ASE_OK;
        return ASE_NoClock;
    }


    ASIOError RouteASIO::getSampleRate(ASIOSampleRate* sampleRate) {
        LOG_CTX(RouteASIO::getSampleRate, "get sample rate {}hz", *sampleRate);
        *sampleRate = routeClient->get_sample_rate();
        return ASE_OK;
    }


    ASIOError RouteASIO::setSampleRate(ASIOSampleRate sampleRate) {
        LOG_CTX(RouteASIO::setSampleRate, "set sample rate {}hz", sampleRate);

        if (sampleRate != routeClient->get_sample_rate())
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
//        DBG_CTX(RouteASIO::getChannelInfo, "{} channel #{} named {}", info->is_input ? "input" : "output", info->channel,
//                info->name);
        if (info->channel < 0 || info->channel >= routeClient->get_channel_count())
            return ASE_InvalidParameter;

        info->type = ASIOSTFloat32LSB;
        info->channelGroup = 0;
        info->isActive = ASIOFalse;

        // get info
        channel_info ch = routeClient->get_channel_info(info->isInput, info->channel);

        // set values from config
        memcpy(info->name, ch.name, 32 * sizeof(char));
        info->isActive = ch.active ? ASIOTrue : ASIOFalse;

        return ASE_OK;
    }


    ASIOError RouteASIO::createBuffers(ASIOBufferInfo* bufferInfos, long numChannels,
                                       long bufferSize, ASIOCallbacks* callbacks) {
        DBG_CTX(RouteASIO::createBuffers, "numChannels={}, bufferSize={}", numChannels, bufferSize);

        ASIOBufferInfo* info = bufferInfos;

        blockFrames = routeClient->get_buffer_size();

        // initialize buffers
        for (int i = 0; i < routeClient->get_channel_count(); i++) {
            inputBuffers[i] = new float[routeClient->get_buffer_size() * 2];
            outputBuffers[i] = new float[routeClient->get_buffer_size() * 2];

            memset(inputBuffers[i], 0, routeClient->get_buffer_size() * 2 * sizeof(float));
            memset(outputBuffers[i], 0, routeClient->get_buffer_size() * 2 * sizeof(float));
        }

        for (int i = 0; i < numChannels; i++, info++) {
            if (info->channelNum < 0 || info->channelNum >= routeClient->get_channel_count()) {
                CRT_CTX(RouteASIO::createBuffers, "channel index {0} out of range", info->channelNum);
            }

            // map buffers to correct pointers
            if (info->isInput) {
                info->buffers[0] = inputBuffers[info->channelNum];
                info->buffers[1] = inputBuffers[info->channelNum] + blockFrames;
            } else {
                info->buffers[0] = outputBuffers[info->channelNum];
                info->buffers[1] = outputBuffers[info->channelNum] + blockFrames;
            }
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

        callbacks = 0;
        stop();

//        for (int i = 0; i < MAX_CHANNELS; i++) {
//            delete [] inputBuffers[i];
//            delete [] outputBuffers[i];
//        }

//        for (i = 0; i < activeInputs; i++)
//            delete inputBuffers[i];
//        activeInputs = 0;
//
//        for (i = 0; i < activeOutputs; i++)
//            delete outputBuffers[i];
//        activeOutputs = 0;
        return ASE_OK;
    }


    ASIOError RouteASIO::controlPanel() {
        DBG_CTX(RouteASIO::controlPanel, "opening control panel...");
        routeClient->open_config();
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
            default:
                DBG_CTX(RouteASIO::future, "unknown");
        }
        return ASE_NotPresent;
    }

    void RouteASIO::processInput() {
        for (int i = 0; i < routeClient->get_channel_count(); i++) {

            // get the buffer
            buffer_info* buf = routeClient->get_buffer(true, i);

            if (buf == nullptr) continue;

            // copy values over
            if (toggle) {
                memcpy(inputBuffers[i] + blockFrames, buf->buffer2, blockFrames * sizeof(float));
            } else {
                memcpy(inputBuffers[i], buf->buffer1, blockFrames * sizeof(float));
            }
        }
    }

    void RouteASIO::processOutput() {
        for (int i = 0; i < routeClient->get_channel_count(); i++) {

            // get the buffer
            buffer_info* buf = routeClient->get_buffer(false, i);

            if (buf == nullptr) continue;

            // copy values over
            if (toggle) {
                memcpy(buf->buffer2, outputBuffers[i] + blockFrames, blockFrames * sizeof(float));
                buf->toggle = true;
            } else {
                memcpy(buf->buffer1, outputBuffers[i], blockFrames * sizeof(float));
                buf->toggle = false;
            }
        }

/*        for (int i = 0; i < routeClient->getChannelCount(); i++) {
            memcpy(routeClient->getBuffer(true, i)->buffer1, routeClient->getBuffer(false, i)->buffer1, blockFrames * sizeof(float));
            memcpy(routeClient->getBuffer(true, i)->buffer2, routeClient->getBuffer(false, i)->buffer2, blockFrames * sizeof(float));
        }*/

    }

    void RouteASIO::updateState(RunState newState) {
        DBG_CTX(RouteASIO::updateState,"driver state change from {}->{}", stateToString(state), stateToString(newState));

        // update state
        state = newState;

    }

    RunState RouteASIO::getState() const {
        return state;
    }

    void RouteASIO::bufferSwitch() {
        // break if not running
        if (getState() != RunState::RUNNING) return;

        if (started && callbacks) {
            // trigger debugger buffer switch tick
//            dbg->bufferTick();

            // latch to system time
            clock->latchTime(&theSystemTime);

            // process I/O
            processInput();
            processOutput();

            // update the sample position
            samplePosition += blockFrames;

            if (timeInfoMode)
                bufferSwitchX();
            else
                callbacks->bufferSwitch(toggle, ASIOFalse);

            // flip flop the toggle
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

    ASIODebugger::ASIODebugger(RouteASIO *asio, int loopTime): thread(this, "asio_debugger"), asio(asio), loopTime(loopTime) {
        DBG_CTX(ASIODebugger::new, "running with period of {0}ms", loopTime);
    }

    ASIODebugger::~ASIODebugger() {
        DBG_CTX(ASIODebugger::~, "");
    }

    void ASIODebugger::start() {
        DBG_CTX(ASIODebugger::start, "");

        // set starting time
        lastTime = std::chrono::high_resolution_clock::now();

        // start the thread
//        thread.start();
    }

    void ASIODebugger::stop() {
        DBG_CTX(ASIODebugger::stop, "");

        // stop the thread
        thread.stop();
    }

    void ASIODebugger::bufferTick() {

        // get current time
        const auto nowTime = std::chrono::high_resolution_clock::now();

        // calculate difference
        const auto dur = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime).count();
        lastDiff = dur;

        // calculate cumulative moving average
        const double old = avgBufferTimeMsec;
        avgBufferTimeMsec = old + (dur - old) / ( count + 1 );

        // increment count
        count++;

        // set last time
        lastTime = std::chrono::high_resolution_clock::now();
    }

    STATUS ASIODebugger::init() {
        return Runnable::init();
    }

    STATUS ASIODebugger::execute() {
        // sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(loopTime));

        // print average buffer time
        DBG_CTX(ASIODebugger::execute, "{3}smp/{1}hz, last: {4}, avg: {0}ms, tot: {2}ms",
                avgBufferTimeMsec/1000.0,
                asio->sampleRate,
                (double (asio->blockFrames)) / asio->sampleRate * 1000,
                asio->blockFrames,
                lastDiff/1000.0);

        return STATUS_OK;
    }

}


double AsioSamples2double(ASIOSamples* samples) {
    double a = (double) (samples->lo);
    if (samples->hi)
        a += (double) (samples->hi) * twoRaisedTo32;
    return a;
}

