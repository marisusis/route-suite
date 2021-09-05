#include "ServerChannel.h"
#include "utils.h"
#include "PipeRequest.h"
#include "server/RouteServer.h"

namespace Route {

    HANDLE ChannelConnectionThread::mutexHandle = nullptr;

    ServerChannel::ServerChannel() : thread(this, "ServerChannel") {
        LOG_CTX(ServerChannel::new, "new server channel.");
    }

    ServerChannel::~ServerChannel() {
        DBG_CTX(ServerChannel::~, "");

        // create an iterator for the list of clients
        std::list<ChannelConnectionThread*>::iterator  iter;

        // shut down the clients
        for (iter = clients.begin(); iter != clients.end(); iter++) {

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

        return STATUS_OK;
    }

    ChannelConnectionThread::ChannelConnectionThread(PipeClient* pipe) : pipe(pipe), thread(this, "channel") {
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

        // lock mutex
        if (WaitForSingleObject(mutexHandle, INFINITE) == WAIT_FAILED) {
            ERR_CTX(ChannelConnectionThread::execute, "error locking mutex!");
            return STATUS_MUTEX_ERROR;
        }

        if (readStatus != STATUS_OK) {
            ERR_CTX(ChannelConnectionThread::execute, "unable to decode request header!");
            return STATUS_BAD_REQUEST;
        } else {
            LOG_CTX(ChannelConnectionThread::execute, "TODO decoding header...");

            // TODO a better way
            // handle request
            return decoder->handleRequest(pipe, requestHeader.type);
        }
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
            decoder = new RequestDecoder(server);

            return STATUS_OK;
        }
    }

    STATUS ChannelConnectionThread::close() {
        DBG_CTX(ChannelConnectionThread::close, "closing channel connection thread...");

        // close the pipe
        pipe->close();

        return STATUS_NO_THREAD;
    }

    bool ChannelConnectionThread::isRunning() {
        return false;
    }
}


