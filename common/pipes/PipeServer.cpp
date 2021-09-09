#include <constants.h>
#include "PipeServer.h"
#include "utils.h"
#include "PipeClient.h"

namespace Route {
    PipeServer::PipeServer() : Pipe() {
        DBG_CTX(PipeServer::new, "new PipeServer.");
    }

    PipeServer::~PipeServer() {
        DBG_CTX(PipeServer::~, "deconstruct PipeServer.");

    }

    STATUS PipeServer::bind(const char* theNamespace, int which) {
        return bind(theNamespace, nullptr, which);
    }

    STATUS PipeServer::bind(const char* theNamespace, const char* name, int which) {
        // check that a name is given
        if (name == nullptr) {
            // create pipe name string
            snprintf(pipeName, sizeof(pipeName), R"(\\.\pipe\%s_%s_pipe_%s_%d)", GLOBAL_NAMESPACE_PREFIX, theNamespace,
                     PIPE_NAME_DEFAULT, which);
        } else {
            // create generic pipe name
            WRN_CTX(PipeServer::bind, "no name provided for pipe!");
            snprintf(pipeName, sizeof(pipeName), R"(\\.\pipe\%s_%s_pipe_%s_%d)", GLOBAL_NAMESPACE_PREFIX, theNamespace,
                     name, which);
        }

        LOG_CTX(PipeServer::bind, "binding to pipe [{}]...", pipeName);

        // create named pipe and get our handle
        pipeHandle = CreateNamedPipe(
                pipeName, // the name of our pipe
                PIPE_ACCESS_DUPLEX , // two way pipe, | FILE_FLAG_OVERLAPPED for async
                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // TODO something
                PIPE_UNLIMITED_INSTANCES, // allow unlimited instances
                PIPE_BUFFER_SIZE,
                PIPE_BUFFER_SIZE,
                INFINITE,
                NULL
        );

        // make sure our pipe handle is valid
        if (pipeHandle == INVALID_HANDLE_VALUE) {
            ERR_CTX(PipeServer::bind, "unable to create pipe [{}]!", pipeName);
            return STATUS_ERROR;
        }

        return STATUS_OK;
    }

    STATUS PipeServer::waitAccept() {
        // wait for a client to connect; accept
        if (ConnectNamedPipe(pipeHandle, NULL)) {
            return STATUS_OK;
        } else {
            if (GetLastError() == ERROR_PIPE_CONNECTED) {
                WRN_CTX(PipeServer::waitAccept, "pipe [{}] already connected!", pipeName);
                return STATUS_OK;
            } else {
                ERR_CTX(PipeServer::waitAccept, "cannot accept client to pipe [{}], err={}", pipeName, GetLastError());
                return STATUS_ERROR;
            }
        }
    }

    STATUS PipeServer::waitAcceptClient(PipeClient* client) {
        // wait for a client to connect; accept
        if (ConnectNamedPipe(pipeHandle, NULL)) {
            new (client) PipeClient(pipeHandle, pipeName);

            // init pipe to default value
            pipeHandle = INVALID_HANDLE_VALUE;

            return STATUS_OK;
        } else {
            if (GetLastError() == ERROR_PIPE_CONNECTED) {
                WRN_CTX(PipeServer::waitAcceptClient, "pipe [{}] already connected!", pipeName);
                new (client) PipeClient(pipeHandle, pipeName);
                return STATUS_OK;
            } else {
                ERR_CTX(PipeServer::waitAcceptClient, "cannot accept client to pipe [{}], err={}", pipeName, GetLastError());
                client = nullptr;
                return STATUS_ERROR;
            }
        }
    }

    STATUS PipeServer::close() {
        LOG_CTX(PipeServer::close, "closing pipe server [{}]...", pipeName);

        // check for a valid pipe handle
        if (pipeHandle != INVALID_HANDLE_VALUE) {
            // disconnect clients from pipe

            DBG_CTX(PipeServer::close, "disconnecting clients [{}]...", pipeName);
            DisconnectNamedPipe(pipeHandle);

            // close the pipe handle
            DBG_CTX(PipeServer::close, "closing handle [{}]...", pipeName);
            CloseHandle(pipeHandle);

            // set to invalid for posterity
            pipeHandle = INVALID_HANDLE_VALUE;

            return STATUS_OK;
        } else {
            ERR_CTX(PipeServer::close, "unable to close pipe, pipe doesn't exist!");
            return STATUS_ERROR;
        }
    }
}