#ifndef ROUTE_SUITE_PIPESERVER_H
#define ROUTE_SUITE_PIPESERVER_H

#include "types.h"
#include "Pipe.h"
#include "PipeClient.h"

namespace Route {
    class PipeServer : public Pipe {

    public:
        PipeServer();
        ~PipeServer();

        STATUS bind(const char* theNamespace, int which);
        STATUS bind(const char* theNamespace, const char* name, int which);
        STATUS waitAccept();
        STATUS waitAcceptClient(PipeClient* client);

        STATUS close();

    };
}

#endif //ROUTE_SUITE_PIPESERVER_H
