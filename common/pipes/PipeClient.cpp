#include "PipeClient.h"
#include "utils.h"

namespace Route {

    PipeClient::PipeClient() {
        LOG_CTX(PipeClient::new, "new PipeClient.");
    }

    PipeClient::PipeClient(HANDLE pipe_handle, const char* pipe_name) : Pipe(pipe_handle) {
        LOG_CTX(PipeClient::new, "new pipe client with handle named [{}].", pipe_name);
        strcpy_s(pipeName, PIPE_NAME_SIZE, pipe_name);
    }

    PipeClient::~PipeClient() {
        LOG_CTX(PipeClient::~, "deleting pipe client for [{}]...", pipeName);
    }

    STATUS PipeClient::connect(const char* theNamespace, int which) {
        return connect(theNamespace, nullptr, which);
    }

    STATUS PipeClient::connect(const char* theNamespace, const char* name, int which) {
        // check that a name is given
        if (name == nullptr) {
            // create pipe name string
            snprintf(pipeName, sizeof(pipeName), R"(\\.\pipe\%s_%s_pipe_%s_%d)", GLOBAL_NAMESPACE_PREFIX, theNamespace,
                     PIPE_NAME_DEFAULT, which);
        } else {
            // create generic pipe name
            WRN_CTX(PipeClient::connect, "no name provided for pipe!");
            snprintf(pipeName, sizeof(pipeName), R"(\\.\pipe\%s_%s_pipe_%s_%d)", GLOBAL_NAMESPACE_PREFIX, theNamespace,
                     name, which);
        }

        LOG_CTX(PipeClient::connect, "connecting to pipe [{}]...", pipeName);


        while (true) {

            // open the named pipe file
            pipeHandle = CreateFile(
                    pipeName, // pipe file path
                    GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL
                    );

            // we're all good to go if the pipe handle is valid
            if (pipeHandle != INVALID_HANDLE_VALUE) {
                return STATUS_OK;
            }

            // if another error besides missing or busy pipe, we failed
            if ((GetLastError() != ERROR_PIPE_BUSY) && (GetLastError() != ERROR_FILE_NOT_FOUND)) {
                CRT_CTX(PipeClient::connect, "({0}) cannot connect to pipe [{1}]!", GetLastError(), pipeName);
                return STATUS_ERROR;
            }

            // pipe is busy
            if (!WaitNamedPipe(pipeName, 2000)) {
                CRT_CTX(PipeClient::connect, "({0}) cannot connect to pipe [{1}] after wait!", GetLastError(), pipeName);
                return STATUS_ERROR;
            }

        }

    }

    STATUS PipeClient::close() {
        // check if handle is valid
        if (pipeHandle != INVALID_HANDLE_VALUE) {
            // close our pipe handle
            CloseHandle(pipeHandle);

            // set to invalid handle to cover our bases
            pipeHandle = INVALID_HANDLE_VALUE;

            return STATUS_OK;
        } else {
            // nothing to close
            return STATUS_ERROR;
        }
    }

}