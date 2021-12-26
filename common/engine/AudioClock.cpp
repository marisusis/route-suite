#include "AudioClock.h"
#include "RouteEngine.h"
#include "utils.h"

Route::AudioClock::AudioClock(RouteEngine* engine) : engine(engine), thread(this, "AudioClock", true) {
    DBG_CTX(AudioClock::new, "");
}

Route::AudioClock::~AudioClock() {
    DBG_CTX(AudioClock::~, "");
}


STATUS Route::AudioClock::open(int sample_rate, int buffer_size) {
    LOG_CTX(AudioClock::open, "opened clock with sampleRate={}, bufferSize={}", sample_rate, buffer_size);

    // assign values
    this->sampleRate = sample_rate;
    this->bufferSize = buffer_size;

    return STATUS_OK;
}

STATUS Route::AudioClock::close() {
    DBG_CTX(AudioClock::close, "closing audio clock...");

    // stop the clock
    this->stop();

    return STATUS_OK;
}

STATUS Route::AudioClock::start() {
    LOG_CTX(AudioClock::start, "starting audio clock...");

    thread.start();

    return STATUS_OK;
}

STATUS Route::AudioClock::stop() {
    LOG_CTX(AudioClock::stop, "stopping audio clock...");

    // stop the audio clock thread
    thread.stop();

    return STATUS_OK;
}

STATUS Route::AudioClock::init() {

    waitTime = std::chrono::duration<double>(bufferSize/sampleRate);

    LOG_CTX(ASIOClock::init, "sleep time {0}s", waitTime.count());

    // update last time
    lastTime = std::chrono::high_resolution_clock::now();

    return STATUS_OK;
}

STATUS Route::AudioClock::execute() {

    // since we are running in a realtime state, we'll break when the thread is "no longer running"
    if (thread.getState() != RUNNING) return STATUS_BREAK;

    // keep looping until it's time
    if ((std::chrono::high_resolution_clock::now() - lastTime) < waitTime) {
        return STATUS_OK;
    }

    // update last time
    lastTime = std::chrono::high_resolution_clock::now();

    // TODO tick the engine
    engine->tick();

    return STATUS_OK;
}
