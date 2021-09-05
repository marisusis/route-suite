#ifndef ROUTE_SUITE_ROUTESERVER_H
#define ROUTE_SUITE_ROUTESERVER_H

#include <pipes/ServerChannel.h>
#include "types.h"

namespace Route {

    class RouteServer {

    private:
        ServerChannel requestChannel;
        int currentReferenceNumber = 1;

    public:
        RouteServer();
        ~RouteServer();

        STATUS open();
        STATUS close();

        STATUS start();
        STATUS stop();

        STATUS tempAction(std::string action);

        int getNewReferenceNumber();

    };

}


#endif //ROUTE_SUITE_ROUTESERVER_H
