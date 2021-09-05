#ifndef ROUTE_SUITE_REQUESTDECODER_H
#define ROUTE_SUITE_REQUESTDECODER_H

#include "types.h"
#include "PipeClient.h"

namespace Route {

    class RouteServer;

    class RequestDecoder {

    private:
        RouteServer* server;

    public:
        RequestDecoder(RouteServer* the_server);
        ~RequestDecoder();

        STATUS handleRequest(PipeClient* pipe, int type);

    };

}


#endif //ROUTE_SUITE_REQUESTDECODER_H
