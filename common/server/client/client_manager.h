#ifndef ROUTE_SUITE_CLIENT_MANAGER_H
#define ROUTE_SUITE_CLIENT_MANAGER_H

#include <map>
#include "shared/shared_structures.h"
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

    class client_manager {

    private:
        std::map<int, Client*> clients;
        route_server& server;
        boolean activeRefs[MAX_CLIENTS];
        client_info* shmClients;
        shared_memory_object shm_clients;
        mapped_region shm_clients_region;

        STATUS free_ref(int ref);
        STATUS allocate_ref(int& ref);

    public:
        client_manager(route_server* server);
        ~client_manager();

        STATUS open();
        STATUS close();

        STATUS add_client(std::string clientName, const int& pid, int* ref);

        STATUS close_client(const int ref);

        buffer_info* get_buffer_info(int ref, int channel, bool input);

        client_info* get_client_info(int ref);

        std::map<int, Client*>* getClients();
    };

}

#endif //ROUTE_SUITE_CLIENT_MANAGER_H
