#ifndef ROUTE_SUITE_ROUTESERVER_H
#define ROUTE_SUITE_ROUTESERVER_H

#include <pipes/ServerChannel.h>
#include "server/client/ClientManager.h"
#include "types.h"

namespace Route {

    class RouteServer {

    private:
        ServerChannel requestChannel;
        ClientManager clientManager;
        int currentReferenceNumber = 1;

    public:
        RouteServer();
        ~RouteServer();

        STATUS open();
        STATUS close();

        STATUS start();
        STATUS stop();

        ClientManager* getClientManager();

        STATUS tempAction(const std::string& action);

        int getNewReferenceNumber();

    };

}


#endif //ROUTE_SUITE_ROUTESERVER_H
