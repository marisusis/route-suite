#include "audio_clock.h"
#include "route_engine.h"
#include "utils.h"

route::audio_clock::audio_clock(route_engine* engine) : engine(engine), thread(this, "AudioClock", true) {
    DBG_CTX(audio_clock::new, "");
}

route::audio_clock::~audio_clock() {
    DBG_CTX(audio_clock::~, "");
}


STATUS route::audio_clock::open(int sample_rate, int buffer_size) {
    LOG_CTX(audio_clock::open, "opened clock with sampleRate={}, bufferSize={}", sample_rate, buffer_size);

    // assign values
    this->sampleRate = sample_rate;
    this->bufferSize = buffer_size;

    return STATUS_OK;
}

STATUS route::audio_clock::close() {
    DBG_CTX(audio_clock::close, "closing audio clock...");

    // stop the clock
    this->stop();

    return STATUS_OK;
}

STATUS route::audio_clock::start() {
    LOG_CTX(audio_clock::start, "starting audio clock...");

    thread.start();

    return STATUS_OK;
}

STATUS route::audio_clock::stop() {
    LOG_CTX(audio_clock::stop, "stopping audio clock...");

    // stop the audio clock thread
    thread.stop();

    return STATUS_OK;
}

STATUS route::audio_clock::init() {

    waitTime = std::chrono::duration<double>(bufferSize/sampleRate);

    LOG_CTX(ASIOClock::init, "sleep time {0}s", waitTime.count());

    // update last time
    lastTime = std::chrono::high_resolution_clock::now();

    return STATUS_OK;
}

STATUS route::audio_clock::execute() {

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
