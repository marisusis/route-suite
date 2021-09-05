#ifndef ROUTE_SUITE_PIPECLIENT_H
#define ROUTE_SUITE_PIPECLIENT_H

#include "Pipe.h"

namespace Route {
    class PipeClient : public Pipe {

    public:
        PipeClient();
        PipeClient(HANDLE pipe_handle, const char* pipe_name);
        ~PipeClient();

        STATUS connect(const char* theNamespace, int which);
        STATUS connect(const char* theNamespace, const char* name, int which);

        STATUS close();

    };
}


#endif //ROUTE_SUITE_PIPECLIENT_H
