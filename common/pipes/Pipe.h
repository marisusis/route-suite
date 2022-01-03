#ifndef ROUTE_SUITE_PIPE_H
#define ROUTE_SUITE_PIPE_H

#include <windows.h>
#include "types.h"
#include "constants.h"
#include "utils.h"

namespace route {

    class Pipe {

    protected:
        HANDLE pipeHandle;
        char pipeName[PIPE_NAME_SIZE] = "NO_NAME";

    public:
        Pipe(): pipeHandle(INVALID_HANDLE_VALUE) {
            DBG_CTX(Pipe::new, "new Pipe with no handle");
        };
        explicit Pipe(HANDLE pipe): pipeHandle(pipe) {
            DBG_CTX(Pipe::new, "new Pipe with handle");
        };

        STATUS read(void* data, int size);
        STATUS write(void* data, int size);

    };

}

#endif //ROUTE_SUITE_PIPE_H
