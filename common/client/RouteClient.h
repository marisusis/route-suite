#ifndef ROUTE_SUITE_ROUTECLIENT_H
#define ROUTE_SUITE_ROUTECLIENT_H

#include "shared/SharedStructures.h"
#include "pipes/ClientChannel.h"
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

    class RouteClient {

    private:
        ClientChannel channel;
        std::string clientName = "NO_NAME";
        int ref = -1;
        shared_memory_object shm_info;
        mapped_region shm_info_region;
        route_info* info;

    public:
        RouteClient(const std::string client_name);
        ~RouteClient();

        STATUS open();
        STATUS close();

        STATUS openConfig();

        int getRef() const;

        int getSampleRate() const;
        int getBufferSize() const;

    };

}

#endif //ROUTE_SUITE_ROUTECLIENT_H
