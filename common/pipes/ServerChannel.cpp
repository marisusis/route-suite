#include "ServerChannel.h"
#include "utils.h"
#include "PipeRequest.h"
#include "server/RouteServer.h"

namespace Route {

    HANDLE ChannelConnectionThread::mutexHandle = nullptr;

    ServerChannel::ServerChannel() : thread(this, "server_channel") {
        LOG_CTX(ServerChannel::new, "new server channel.");
    }

    ServerChannel::~ServerChannel() {
        DBG_CTX(ServerChannel::~, "");

        // create an iterator for the list of clients
        std::list<ChannelConnectionThread*>::iterator iter;

        // shut down the clients
        for (iter = channelConnections.begin(); iter != channelConnections.end(); iter++) {

            // current value
            ChannelConnectionThread* chan = *iter;

            // close the client
            chan->close();

            // delete the client
            delete chan;
        }

    }

    STATUS ServerChannel::open(RouteServer* the_server, const char* server_name) {

        // TODO not complete, see jack2

        LOG_CTX(ServerChannel::open, "opening server channel for server [{}]...", server_name);

        // set server value
        ServerChannel::server = the_server;

        // set server name
        memcpy(serverName, server_name, sizeof(serverName));

        return STATUS_OK;
    }

    STATUS ServerChannel::close() {
        // close the pipe
        return requestPipe.close();
    }

    STATUS ServerChannel::start() {
        STATUS startStatus = thread.start();

        if (startStatus != STATUS_OK) {
            ERR_CTX(ServerChannel::start, "error starting server channel thread!");
            return startStatus;
        } else {
            return STATUS_OK;
        }
    }

    STATUS ServerChannel::stop() {
        return thread.kill();
    }

    STATUS ServerChannel::init() {
        return Runnable::init();
    }

    STATUS ServerChannel::execute() {
        // listen for clients
        STATUS listenStatus = listenForClients();

        if (listenStatus != STATUS_OK) {
            return listenStatus;
        }

        return acceptClients();
    }

    STATUS ServerChannel::listenForClients() {
        // bind
        STATUS bindStatus = requestPipe.bind(PIPE_SERVER_TAG, serverName, 0);

        if (bindStatus != STATUS_OK) {
            ERR_CTX(ServerChannel::listenForClients, "cannot bind to request pipe!");
            return bindStatus;
        } else {
            return STATUS_OK;
        }
    }

    STATUS ServerChannel::acceptClients() {
        // pipe client
        PipeClient* pipe = static_cast<PipeClient*>(malloc(sizeof(PipeClient)));

        // accept the client
        STATUS acceptStatus = requestPipe.waitAcceptClient(pipe);

        // make sure we all good
        if (acceptStatus != STATUS_OK) {
            ERR_CTX(ServerChannel:acceptClients, "cannot connect pipe!");
            return acceptStatus;
        }

        // add the client
        addClient(pipe);

        return STATUS_OK;
    }

    STATUS ServerChannel::addClient(PipeClient* pipe) {
        // clients iterator
        std::list<ChannelConnectionThread*>::iterator iter = channelConnections.begin();

        // the thread
        ChannelConnectionThread* channelThread;

        DBG_CTX(ServerChannel::addClient, "adding client, current size={}", channelConnections.size());

        // remove dead/not running clients
        while (iter != channelConnections.end()) {
            // current client
            channelThread = *iter;

            // check if running
            if (channelThread->isRunning()) {
                // next item
                iter++;
            } else {
                // delete client
                iter = channelConnections.erase(iter);
                delete channelThread;
            }
        }

        // create new thread
        channelThread = new ChannelConnectionThread(pipe);

        // open the thread
        channelThread->open(server);

        // add to list
        channelConnections.push_back(channelThread);

        return STATUS_OK;
    }

    ChannelConnectionThread::ChannelConnectionThread(PipeClient* pipe) : pipe(pipe), thread(this, "channel_connection") {
        // allocate mutex
        if (mutexHandle == nullptr) {
            DBG_CTX(ChannelConnectionThread::new, "creating static mutex...");
            mutexHandle = CreateMutex(nullptr, FALSE, nullptr);
        }
    }

    ChannelConnectionThread::~ChannelConnectionThread() {
        DBG_CTX(ChannelConnectionThread::~, "deleting pipe");
        delete pipe;
    }

    STATUS ChannelConnectionThread::init() {
        return Runnable::init();
    }

    STATUS ChannelConnectionThread::execute() {

        // pipe request header
        PipeRequest requestHeader;

        // read the request header
        STATUS readStatus = requestHeader.read(pipe);
        STATUS returnStatus = STATUS_OK;

        // lock mutex
        if (WaitForSingleObject(mutexHandle, INFINITE) == WAIT_FAILED) {
            ERR_CTX(ChannelConnectionThread::execute, "error locking mutex!");
            return STATUS_MUTEX_ERROR;
        }

        if (readStatus != STATUS_OK) {
            ERR_CTX(ChannelConnectionThread::execute, "unable to decode request header!");
            returnStatus = STATUS_BAD_REQUEST;
        } else {
            LOG_CTX(ChannelConnectionThread::execute, "TODO decoding header...");

            // TODO a better way
            // handle request
            returnStatus = decoder->handleRequest(pipe, requestHeader.type);
        }

        // release the mutex
        if (!ReleaseMutex(mutexHandle)) {
            CRT_CTX(ChannelConnectionThread::execute, "error releasing mutex!");
            return STATUS_ERROR;
        }

        return returnStatus;
    }

    STATUS ChannelConnectionThread::open(RouteServer* the_server) {
        DBG_CTX(ChannelConnectionThread::open, "opening channel thread...");

        // start the thread
        STATUS startStatus = thread.start();
        if (startStatus != STATUS_OK) {
            // TODO create decoder?
            ERR_CTX(ChannelConnectionThread::open, "cannot start channel connection thread!");
            return startStatus;
        } else {

            // get server
            server = the_server;

            // create request decoder
            decoder = new RequestDecoder(server, this);

            return STATUS_OK;
        }
    }

    STATUS ChannelConnectionThread::close() {
        DBG_CTX(ChannelConnectionThread::close, "closing channel connection thread...");

        // kill the thread
        thread.kill();

        // close the pipe
        pipe->close();

        return STATUS_NO_THREAD;
    }

    bool ChannelConnectionThread::isRunning() {
        WRN_CTX(ChannelConnectionThread::isRunning, "not implemented!");
        return true;
    }
}


