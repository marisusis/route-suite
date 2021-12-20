#ifndef ROUTE_SUITE_ROUTESERVER_H
#define ROUTE_SUITE_ROUTESERVER_H

#include <pipes/ServerChannel.h>
#include "server/client/ClientManager.h"
#include "types.h"
#include "boost/interprocess/shared_memory_object.hpp"
#include "boost/interprocess/mapped_region.hpp"

using boost::interprocess::shared_memory_object;
using boost::interprocess::mapped_region;
using boost::interprocess::create_only;
using boost::interprocess::open_or_create;
using boost::interprocess::open_only;
using boost::interprocess::read_only;
using boost::interprocess::read_write;


namespace Route {

    class RouteServer {

    private:
        ServerChannel requestChannel;
        ClientManager clientManager;
        int currentReferenceNumber = 1;
        shared_memory_object shm_info;
        mapped_region shm_info_region;


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
