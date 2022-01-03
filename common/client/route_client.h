#ifndef ROUTE_SUITE_ROUTE_CLIENT_H
#define ROUTE_SUITE_ROUTE_CLIENT_H

#include "shared/shared_structures.h"
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

namespace route {

    enum ClientStatus {

        OPEN,
        OPENING,
        CLOSING,
        CLOSED

    };

    class route_client {

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
        client_info clientInfo;
        server_info* info;
        ClientStatus state = CLOSED;

        // buffers
        buffer_info* buffers = nullptr;


    public:
        route_client(std::string client_name);
        ~route_client();

        STATUS open();
        STATUS close();

        STATUS open_config();

        channel_info get_channel_info(bool input, int index);

        int get_ref() const;

        int get_sample_rate() const;
        int get_buffer_size() const;
        int get_channel_count() const;

        int get_output_latency() const;
        int get_input_latency() const;

        ClientStatus getState() const;

        buffer_info* get_buffer(bool input, int index);

    };

}

#endif //ROUTE_SUITE_ROUTE_CLIENT_H
