#include "shared/SharedStructures.h"
#include "utils.h"
#include "RouteServer.h"
#include "server/client/ClientManager.h"

namespace Route {

    RouteServer::RouteServer() : clientManager(this), bufferManager(this) {
        LOG_CTX(RouteServer::new, "");

        // remove shared memory objects in case they exist
        shared_memory_object::remove(ROUTE_SHM_INFO);
        shared_memory_object::remove(ROUTE_SHM_CLIENTS);

        // created shared memory object
        DBG_CTX(RouteServer::new, "creating shared memory...");
        shm_info = shared_memory_object(open_or_create, ROUTE_SHM_INFO, read_write);

        // truncate to the size of datatypes
        DBG_CTX(RouteServer::new, "truncating shared memory to {0} bytes...", sizeof(route_server_info));
        shm_info.truncate(sizeof(route_server_info));

        // create the mapped regions
        DBG_CTX(RouteServer::new, "creating mapped regions");
        shm_info_region = mapped_region(shm_info,
                                        read_write,
                                        0,
                                        sizeof(route_server_info));

        // create the info
        info = static_cast<route_server_info *>(shm_info_region.get_address());
        strcpy(info->name, "RouteServer by Maris");
        strcpy(info->version, "0.0.1");

        // default sample rate and buffer size
        info->sampleRate = 44100;
        info->bufferSize = 1024;
        info->channelCount = MAX_CHANNELS;
    }

    RouteServer::~RouteServer() {
        LOG_CTX(RouteServer::~, "");

        // remove shared memory object
        DBG_CTX(RouteServer::~, "destroying shared memory [{0}]", ROUTE_SHM_INFO);
        shared_memory_object::remove(ROUTE_SHM_INFO);

    }

    STATUS RouteServer::open() {
        LOG_CTX(RouteServer::open, "opening server...");

        // open the request channel
        requestChannel.open(this, SERVER_NAME);

        // open buffer manager
        bufferManager.open();

        // open client manager
        clientManager.open();


        return STATUS_OK;
    }

    STATUS RouteServer::tempAction(const std::string& action) {
        LOG_CTX(RouteServer::tempAction, "A wild Action has appeared! It's name is {}.", action);
        return STATUS_OK;
    }

    STATUS RouteServer::close() {
        LOG_CTX(RouteServer::close, "closing server...");

        // close client manager
        clientManager.close();

        // close buffer manager
        bufferManager.close();

        // close the request channel
        requestChannel.close();

        return STATUS_OK;
    }

    STATUS RouteServer::start() {
        LOG_CTX(RouteServer::start, "starting server...");

        // start the request channel
        return requestChannel.start();
    }

    STATUS RouteServer::stop() {
        LOG_CTX(RouteServer::stop, "stopping server...");

        // alert all clients


        // stop the request channel
        return requestChannel.stop();
    }

    ClientManager* RouteServer::getClientManager() {
        return &clientManager;
    }

    BufferManager *RouteServer::getBufferManager() {
        return &bufferManager;
    }

    route_server_info* RouteServer::getServerInfo() {
        return info;
    }

}