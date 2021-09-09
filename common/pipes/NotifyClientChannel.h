#ifndef ROUTE_SUITE_NOTIFYCLIENTCHANNEL_H
#define ROUTE_SUITE_NOTIFYCLIENTCHANNEL_H

#include "PipeClient.h"

namespace Route {

    class NotifyClientChannel {
    private:
        PipeClient notifyPipe;

    public:
        NotifyClientChannel();

        STATUS open(const char* name);
        STATUS close();

        // TODO notify

    };

}


#endif //ROUTE_SUITE_NOTIFYCLIENTCHANNEL_H
