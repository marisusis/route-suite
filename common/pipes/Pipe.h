#ifndef ROUTE_SUITE_PIPE_H
#define ROUTE_SUITE_PIPE_H

#include <windows.h>
#include "types.h"
#include "constants.h"

namespace Route {

    class Pipe {

    protected:
        HANDLE pipeHandle;
        char pipeName[PIPE_NAME_SIZE] = "NO_NAME";

    public:
        Pipe(): pipeHandle(INVALID_HANDLE_VALUE) {};
        explicit Pipe(HANDLE pipe): pipeHandle(pipe) {};

        STATUS read(void* data, int size);
        STATUS write(void* data, int size);

    };

}

#endif //ROUTE_SUITE_PIPE_H
