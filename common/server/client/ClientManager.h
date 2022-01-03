#ifndef ROUTE_SUITE_CLIENTMANAGER_H
#define ROUTE_SUITE_CLIENTMANAGER_H

#include <map>
#include "shared/SharedStructures.h"
#include "types.h"
#include "Client.h"
#include "boost/interprocess/shared_memory_object.hpp"
#include "boost/interprocess/mapped_region.hpp"

using boost::interprocess::shared_memory_object;
using boost::interprocess::mapped_region;
using boost::interprocess::create_only;
using boost::interprocess::open_or_create;
using boost::interprocess::open_only;
using boost::interprocess::read_only;
using boost::interprocess::read_write;

namespace route {

    class route_server;

    class ClientManager {

    private:
        std::map<int, Client*> clients;
        route_server* server;
        boolean activeRefs[MAX_CLIENTS];
        route_client* shmClients;
        shared_memory_object shm_clients;
        mapped_region shm_clients_region;

        STATUS freeRef(int ref);
        STATUS allocateRef(int& ref);

    public:
        ClientManager(route_server* server);
        ~ClientManager();

        STATUS open();
        STATUS close();

        STATUS addClient(std::string clientName, const int& pid, int* ref);

        STATUS closeClient(const int ref);

        route_client* getClientInfo(int ref);

        std::map<int, Client*>* getClients();
    };

}

#endif //ROUTE_SUITE_CLIENTMANAGER_H
