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

    enum ClientStatus {

        OPEN,
        OPENING,
        CLOSING,
        CLOSED

    };

    class RouteClient {

    private:
        ClientChannel channel;
        std::string clientName = "NO_NAME";
        int ref = -1;
        shared_memory_object shm_info;
        shared_memory_object shm_buffers;
        shared_memory_object shm_clients;
        mapped_region shm_info_region;
        mapped_region shm_buffers_region;
        mapped_region shm_clients_region;
        route_client clientInfo;
        route_server_info* info;
        ClientStatus state = CLOSED;

        // buffers
        route_buffer* buffers = nullptr;


    public:
        RouteClient(std::string client_name);
        ~RouteClient();

        STATUS open();
        STATUS close();

        STATUS openConfig();

        route_channel_info getChannelInfo(bool input, int index);

        int getRef() const;

        int getSampleRate() const;
        int getBufferSize() const;
        int getChannelCount() const;

        int getOutputLatency() const;
        int getInputLatency() const;

        ClientStatus getState() const;

        route_buffer* getBuffer(bool input, int index);

    };

}

#endif //ROUTE_SUITE_ROUTECLIENT_H
