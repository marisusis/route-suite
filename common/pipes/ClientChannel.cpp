#include "ClientChannel.h"

namespace route {

    ClientChannel::ClientChannel() : thread(this, "ClientChannel") {
        DBG_CTX(ClientChannel::new, "creating client channel...");
        requestPipe = new PipeClient();
    }

    ClientChannel::~ClientChannel() {
        DBG_CTX(ClientChannel::~, "deconstructing client channel...");
        delete requestPipe;
    }

    STATUS ClientChannel::open(const char* server_name, const char* name) {
        // TODO get the client
        // connect to the pipe
        STATUS connectStatus = requestPipe->connect(PIPE_SERVER_TAG, server_name, 0);

        if (connectStatus != STATUS_OK) {
            ERR_CTX(ClientChannel::open, "cannot connect to server pipe!");

            // close pipes
            requestPipe->close();
            notificationListenPipe.close();
            return STATUS_NO_PIPE;
        }

        // TODO check name
        WRN_CTX(ClientChannel::open, "check name with server!");

        // bind to the pipe
        STATUS notificationStatus = notificationListenPipe.bind(PIPE_CLIENT_TAG, name, 0);

        if (notificationStatus != STATUS_OK) {
            ERR_CTX(ClientChannel::open, "cannot bind to notification pipe!");

            // close pipes
            requestPipe->close();
            notificationListenPipe.close();
            return STATUS_NO_PIPE;
        }

        return STATUS_OK;
    }

    STATUS ClientChannel::close() {
        LOG_CTX(ClientChannel::close, "closing client channel...");

        // close pipes
        requestPipe->close();
        notificationListenPipe.close();
        thread.stop();

        // TODO ignoring statuses...
        return STATUS_OK;
    }

    STATUS ClientChannel::start() {
        LOG_CTX(ClientChannel::start, "starting client channel...");

        // start the thread
        STATUS startStatus = thread.start();

        if (startStatus != STATUS_OK) {
            ERR_CTX(ClientChannel::start, "cannot start client listener");
            return startStatus;
        } else {
            return STATUS_OK;
        }
    }

    STATUS ClientChannel::stop() {
        LOG_CTX(ClientChannel::stop, "stopping pipe client channel...");
        WRN_CTX(ClientChannel::stop, "issue with WIN32 thread kill, unsafe");
        return thread.kill();
    }

    STATUS ClientChannel::init() {

        WRN_CTX(ClientChannel::init, "not implemented!");

        // accept notification listen pipe
        STATUS acceptStatus = notificationListenPipe.waitAccept();

        if (acceptStatus != STATUS_OK) {
            ERR_CTX(ClientChannel::init, "cannot establish notification pipe");
            return acceptStatus;
        }

        return STATUS_OK;
    }

    STATUS ClientChannel::execute() {
        // TODO read from the pipe

        return STATUS_OK;
    }

    STATUS ClientChannel::serverCall(PipeRequest* request, PipeResult* result) {

        // write request
        LOG_CTX(ClientChannel::serverCall, "writing server request type={}...", request->type);
        STATUS writeStatus = request->write(requestPipe);

        if (!OK(writeStatus)) {
            ERR_CTX(ClientChannel::serverCall, "error writing request to pipe");
            return writeStatus;
        }

        // read result
        LOG_CTX(ClientChannel::serverCall, "reading server result type={}...", request->type);
        STATUS readStatus = result->read(requestPipe);

        if (!OK(readStatus)) {
            ERR_CTX(ClientChannel::serverCall, "error reading result from pipe");
            return readStatus;
        }

        return STATUS_OK;
    }

    STATUS ClientChannel::serverSend(PipeRequest* request) {
        // write request
        LOG_CTX(ClientChannel::serverCall, "writing server request type={}...", request->type);
        STATUS writeStatus = request->write(requestPipe);

        if (!OK(writeStatus)) {
            ERR_CTX(ClientChannel::serverCall, "error writing request to pipe");
            return writeStatus;
        }

        return STATUS_OK;
    }

    bool ClientChannel::isChannelThread() {
        return thread.isThread();
    }

}