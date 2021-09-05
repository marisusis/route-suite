#ifndef ROUTE_SUITE_REQUESTDECODER_H
#define ROUTE_SUITE_REQUESTDECODER_H

#include "types.h"
#include "PipeClient.h"

namespace Route {

    class RouteServer;
    class ChannelConnectionThread;

    class RequestDecoder {

    private:
        RouteServer* server;
        ChannelConnectionThread* channelThread;

    public:
        RequestDecoder(RouteServer* the_server, ChannelConnectionThread* channel_thread);
        ~RequestDecoder();

        STATUS handleRequest(PipeClient* pipe, int type);

    };

}


#endif //ROUTE_SUITE_REQUESTDECODER_H
