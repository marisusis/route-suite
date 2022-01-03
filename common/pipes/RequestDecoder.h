#ifndef ROUTE_SUITE_REQUESTDECODER_H
#define ROUTE_SUITE_REQUESTDECODER_H

#include "types.h"
#include "PipeClient.h"

namespace route {

    class route_server;
    class ChannelConnectionThread;

    class RequestDecoder {

    private:
        route_server& server;
        ChannelConnectionThread* channelThread;

    public:
        RequestDecoder(route_server* the_server, ChannelConnectionThread* channel_thread);
        ~RequestDecoder();

        STATUS handleRequest(PipeClient* pipe, int type);

    };

}


#endif //ROUTE_SUITE_REQUESTDECODER_H
