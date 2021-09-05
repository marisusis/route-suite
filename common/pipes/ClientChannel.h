#ifndef ROUTE_SUITE_CLIENTCHANNEL_H
#define ROUTE_SUITE_CLIENTCHANNEL_H

#include "thread/Thread.h"
#include "types.h"
#include "PipeRequest.h"
#include "PipeServer.h"

namespace Route {

    class ClientChannel : public Runnable {

    private:
        PipeClient* requestPipe;
        PipeServer notificationListenPipe;
        Thread thread;

    public:
        ClientChannel();
        ~ClientChannel();

        STATUS open(const char* server_name, const char* name);
        STATUS close();

        STATUS start();
        STATUS stop();

        STATUS init() final;
        STATUS execute() final;

        STATUS serverCall(PipeRequest* request, PipeResult* result);
        STATUS serverSend(PipeRequest* request);

        bool isChannelThread();



    };

}

#endif //ROUTE_SUITE_CLIENTCHANNEL_H
