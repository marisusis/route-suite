#include <shared/SharedStructures.h>
#include "route_client.h"
#include "utils.h"
#include "constants.h"

namespace route {

    route_client::route_client(const std::string client_name) : clientName(client_name) {
        TRC_CTX(route_client::new, "");
    }

    route_client::~route_client() {
        TRC_CTX(route_client::~, "");
    }

    STATUS route_client::open() {
        DBG_CTX(route_client::open, "opening client...");

        state = ClientStatus::OPENING;

        // open client channel
        STATUS openStatus = channel.open(SERVER_NAME, clientName.c_str());

        // return with bad status if we cannot open
        if (openStatus != STATUS_OK) {
            CRT_CTX(route_client::open, "unable to establish connection with server! status={}", statusToString(openStatus));
            return openStatus;
        }

        // send client open request to server
        ClientOpenRequest req(clientName.c_str(), _getpid());
        ClientOpenResult res;

        STATUS callStatus = channel.serverCall(&req, &res);

        if (callStatus != STATUS_OK) {
            CRT_CTX(route_client::open, "unable to open client!");
            return callStatus;
        }

        // set the ref
        LOG_CTX(route_client::open, "assigned ref {}.", res.referenceNumber);
        ref = res.referenceNumber;

        // load shared memory
        shm_info = shared_memory_object(open_only, ROUTE_SHM_INFO, read_only);
        shm_buffers = shared_memory_object(open_only, ROUTE_SHM_BUFFERS, read_write);
        shm_clients = shared_memory_object(open_only, ROUTE_SHM_CLIENTS, read_only);


        shm_info_region = mapped_region(shm_info,
                                        boost::interprocess::read_only,
                                        0,
                                        sizeof(route_server_info));

        shm_buffers_region = mapped_region(shm_buffers,
                                        boost::interprocess::read_write,
                                        0,
                                           sizeof(buffer_info) * MAX_BUFFERS);

        shm_clients_region = mapped_region(shm_clients,
                                           read_only,
                                           0,
                                           sizeof(client_info) * MAX_CLIENTS);

        // get the client information
        auto* clients = static_cast<client_info *>(shm_clients_region.get_address());
        clientInfo = clients[ref];

        buffers = static_cast<buffer_info *>(shm_buffers_region.get_address());

        // load the info from shared memory
        info = static_cast<route_server_info *>(shm_info_region.get_address());

        LOG_CTX(route_client::open, "connected to [{2}/v{3}]; running at {0}smp/{1}hz", info->bufferSize, info->sampleRate, info->name, info->version);

        // client is now open
        state = ClientStatus::OPEN;

        return STATUS_OK;
    }

    STATUS route_client::close() {
        DBG_CTX(route_client::open, "closing client...");

        // start closing client
        state = ClientStatus::CLOSING;

        ClientCloseRequest req(ref);

        // notify server of closing client
        channel.serverSend(&req);

        // close the server/client channel
        channel.close();

        // client is closed
        state = ClientStatus::CLOSED;

        return STATUS_OK;
    }

    int route_client::get_ref() const {
        return ref;
    }

    STATUS route_client::open_config() {
        // create the request
        OpenConfigRequest req(ref);

        // send it
        channel.serverSend(&req);

        return STATUS_OK;
    }

    int route_client::get_sample_rate() const {
        return info->sampleRate;
    }

    int route_client::get_buffer_size() const {
        return info->bufferSize;
    }

    int route_client::get_channel_count() const {
        return info->channelCount;
    }

    int route_client::get_input_latency() const {
        return clientInfo.inputLatency;
    }

    int route_client::get_output_latency() const {
        return clientInfo.outputLatency;
    }

    buffer_info* route_client::get_buffer(bool input, int index) {
        if (index >= MAX_CHANNELS) {
            CRT_CTX(route_client::get_buffer, "channel index {0} out of range {1}!", index, MAX_CHANNELS);
        }

        int buf = -1;
        if (input) {
            buf = clientInfo.inputBufferMap[index];
        } else {
            buf = clientInfo.outputBufferMap[index];
        }

        if (buf < 0) {
            CRT_CTX(route_client::get_buffer, "no buffer available for [REF {0}] {2} channel {1}", ref, index, input ? "input" : "output");
            return nullptr;
        }

        return &(buffers[buf]);
    }

    channel_info route_client::get_channel_info(bool input, int index) {
        if (index >= MAX_CHANNELS) {
            CRT_CTX(route_client::get_channel_info, "channel index {0} out of range {1}!", index, MAX_CHANNELS);
        }

        if (input) {
            return clientInfo.inputChannels[index];
        } else {
            return clientInfo.outputChannels[index];
        }
    }

    ClientStatus route_client::getState() const {
        return state;
    }


}