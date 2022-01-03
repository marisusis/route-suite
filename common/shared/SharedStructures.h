//
// Created by maris on 12/20/2021.
//

#ifndef ROUTE_SUITE_SHAREDSTRUCTURES_H
#define ROUTE_SUITE_SHAREDSTRUCTURES_H

#include "constants.h"

namespace route {

    struct route_server_info {
        char name[SERVER_NAME_LENGTH];
        char version[VERSION_NAME_LENGTH];
        int sampleRate;
        int bufferSize;
        int channelCount;
    };

    struct route_buffer {
        // allow for a double buffer
        float buffer1[MAX_BUFFER_SIZE];
        float buffer2[MAX_BUFFER_SIZE];
    };

    struct channel_info {
        bool active;
        char name[256];
    };

    struct client_info {
        char name[256];

        bool usesAudio;

        int inputLatency;
        int outputLatency;

        int inputBufferMap[MAX_CHANNELS];
        int outputBufferMap[MAX_CHANNELS];

        channel_info inputChannels[MAX_CHANNELS];
        channel_info outputChannels[MAX_CHANNELS];

        char clockMutexName[MUTEX_NAME_SIZE];
    };





}

#endif //ROUTE_SUITE_SHAREDSTRUCTURES_H
