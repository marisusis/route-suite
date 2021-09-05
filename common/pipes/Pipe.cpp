#include "Pipe.h"
#include "utils.h"

namespace Route {

    STATUS Pipe::read(void* data, int size) {
        // bytes read
        DWORD bytesRead;

        // read from the pipe
        DBG_CTX(Pipe::read, "reading length {} from [{}]...", size, pipeName);
        BOOL read = ReadFile(pipeHandle, data, size, &bytesRead, NULL);

        // check we actually read something
        if (read) {
            // check we read the correct amount
            if (bytesRead == size) {
                return STATUS_OK;
            } else {
                return STATUS_WRONG_PACKET_SIZE;
            }
        } else {
            return STATUS_NO_TRANSACTION;
        }
    }

    STATUS Pipe::write(void* data, int size) {
        // bytes read
        DWORD bytesWritten;

        // read from the pipe
        DBG_CTX(Pipe::write, "writing length {} to [{}]...", size, pipeName);
        BOOL write = WriteFile(pipeHandle, data, size, &bytesWritten, NULL);

        // check we actually read something
        if (write) {
            // check we read the correct amount
            if (bytesWritten == size) {
                return STATUS_OK;
            } else {
                return STATUS_WRONG_PACKET_SIZE;
            }
        } else {
            return STATUS_NO_TRANSACTION;
        }
    }

}