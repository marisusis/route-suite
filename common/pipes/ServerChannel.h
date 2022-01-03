#ifndef ROUTE_SUITE_SERVERCHANNEL_H
#define ROUTE_SUITE_SERVERCHANNEL_H

#include <list>
#include "thread/Thread.h"
#include "PipeServer.h"
#include "PipeClient.h"
#include "RequestDecoder.h"

namespace route {

    class route_server;

    class ChannelConnectionThread : public Runnable {

    private:
        PipeClient* pipe;
        route_server* server;
        RequestDecoder* decoder;
        Thread thread;

        static HANDLE mutexHandle;

    public:
        ChannelConnectionThread(PipeClient* pipe);
        ~ChannelConnectionThread();

        STATUS open(route_server* the_server);
        STATUS close();

        STATUS init() final;

        STATUS execute() final;

        bool isRunning();

    };

    class ServerChannel : public Runnable {

    private:
        char serverName[SERVER_NAME_LENGTH];
        std::list<ChannelConnectionThread*> channelConnections;
        PipeServer requestPipe;
        route_server* server;
        Thread thread;

    public:
        ServerChannel();
        ~ServerChannel();

        STATUS open(route_server* the_server, const char* server_name);
        STATUS close();
        STATUS start();
        STATUS stop();

        STATUS init() final;

        STATUS execute() final;

        STATUS listenForClients();
        STATUS acceptClients();

        STATUS addClient(PipeClient* pipe);

    };

}


#endif //ROUTE_SUITE_SERVERCHANNEL_H
