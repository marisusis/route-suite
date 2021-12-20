#include <shared/SharedStructures.h>
#include "RouteClient.h"
#include "utils.h"
#include "constants.h"

namespace Route {

    RouteClient::RouteClient(const std::string client_name) : clientName(client_name) {
        TRC_CTX(RouteClient::new, "");
    }

    RouteClient::~RouteClient() {
        TRC_CTX(RouteClient::~, "");
    }

    STATUS RouteClient::open() {
        DBG_CTX(RouteClient::open, "opening client...");

        // open client channel
        STATUS openStatus = channel.open(SERVER_NAME, clientName.c_str());

        // return with bad status if we cannot open
        if (openStatus != STATUS_OK) {
            CRT_CTX(RouteClient::open, "unable to establish connection with server! status={}", statusToString(openStatus));
            return openStatus;
        }

        // send client open request to server
        ClientOpenRequest req(clientName.c_str(), _getpid());
        ClientOpenResult res;

        STATUS callStatus = channel.serverCall(&req, &res);

        if (callStatus != STATUS_OK) {
            CRT_CTX(RouteClient::open, "unable to open client!");
            return callStatus;
        }

        // set the ref
        LOG_CTX(RouteClient::open, "assigned ref {}.", res.referenceNumber);
        ref = res.referenceNumber;

        // load shared memory
        shm_info = shared_memory_object(open_only, ROUTE_SHM_INFO, read_only);
        shm_info_region = mapped_region(shm_info,
                                        boost::interprocess::read_only,
                                        0,
                                        sizeof(route_info));

        // load the info from shared memory
        info = static_cast<route_info *>(shm_info_region.get_address());

        LOG_CTX(RouteClient::open, "connected to {2} v{3}; running at {0}smp/{1}hz", info->bufferSize, info->sampleRate, info->name, info->version);

        return STATUS_OK;
    }

    STATUS RouteClient::close() {
        DBG_CTX(RouteClient::open, "closing client...");

        ClientCloseRequest req(ref);

        channel.serverSend(&req);
        
        channel.close();

        return STATUS_OK;
    }

    int RouteClient::getRef() const {
        return ref;
    }

    STATUS RouteClient::openConfig() {
        // create the request
        OpenConfigRequest req(ref);

        // send it
        channel.serverSend(&req);

        return STATUS_OK;
    }

    int RouteClient::getSampleRate() const {
        return info->sampleRate;
    }

    int RouteClient::getBufferSize() const {
        return info->bufferSize;
    }


}