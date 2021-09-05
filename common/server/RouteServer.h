#ifndef ROUTE_SUITE_ROUTESERVER_H
#define ROUTE_SUITE_ROUTESERVER_H

#include <pipes/ServerChannel.h>
#include "types.h"

namespace Route {

    class RouteServer {

    private:
        ServerChannel requestChannel;

    public:
        RouteServer();
        ~RouteServer();

        STATUS open();
        STATUS close();

        STATUS start();
        STATUS stop();

    };

}


#endif //ROUTE_SUITE_ROUTESERVER_H
