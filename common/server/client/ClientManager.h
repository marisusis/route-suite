#ifndef ROUTE_SUITE_CLIENTMANAGER_H
#define ROUTE_SUITE_CLIENTMANAGER_H

#include <map>
#include "types.h"
#include "Client.h"

namespace Route {

    class RouteServer;

    class ClientManager {

    private:
        std::map<int, Client*> clients;
        RouteServer* server;
        boolean activeRefs[MAX_CLIENTS];

        STATUS freeRef(int ref);
        STATUS allocateRef(int& ref);

    public:
        ClientManager(RouteServer* server);
        ~ClientManager();

        STATUS open();
        STATUS close();

        STATUS addClient(std::string clientName, const int& pid, int* ref);

        STATUS closeClient(const int ref);
    };

}

#endif //ROUTE_SUITE_CLIENTMANAGER_H
