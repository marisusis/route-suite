#include "shared/shared_structures.h"
#include "utils.h"
#include "route_server.h"

namespace route {

    route_server::route_server() : clientManager(this), bufferManager(this), audioEngine(this), graphManager(this) {
        LOG_CTX(RouteServerRouteServer::new, "");

        // remove shared memory objects in case they exist
        shared_memory_object::remove(ROUTE_SHM_INFO);
        shared_memory_object::remove(ROUTE_SHM_CLIENTS);

        // created shared memory object
        DBG_CTX(RouteServerRouteServer::new, "creating shared memory...");
        shm_info = shared_memory_object(open_or_create, ROUTE_SHM_INFO, read_write);

        // truncate to the size of datatypes
        DBG_CTX(route_server::new, "truncating shared memory to {0} bytes...", sizeof(server_info));
        shm_info.truncate(sizeof(server_info));

        // create the mapped regions
        DBG_CTX(route_server::new, "creating mapped regions");
        shm_info_region = mapped_region(shm_info,
                                        read_write,
                                        0,
                                        sizeof(server_info));

        // create the info
        info = static_cast<server_info *>(shm_info_region.get_address());
        memcpy(info->name, SERVER_NAME, SERVER_NAME_LENGTH * sizeof(char));
        memcpy(info->version, SERVER_VERSION, VERSION_NAME_LENGTH * sizeof(char));

        // default sample rate and buffer size
        info->sampleRate = 44100;
        info->bufferSize = 1024;
        info->channelCount = MAX_CHANNELS;
    }

    route_server::~route_server() {
        LOG_CTX(route_server::~, "");

        // remove shared memory object
        DBG_CTX(route_server::~, "destroying shared memory [{0}]", ROUTE_SHM_INFO);
        shared_memory_object::remove(ROUTE_SHM_INFO);

    }

    STATUS route_server::open() {
        LOG_CTX(route_server::open, "opening server...");

        // open the request channel
        requestChannel.open(this, SERVER_NAME);

        // open buffer manager
        bufferManager.open();

        // open client manager
        clientManager.open();

        // open the audio engine
        audioEngine.open();

        return STATUS_OK;
    }

    STATUS route_server::tempAction(const std::string& action) {
        LOG_CTX(route_server::tempAction, "A wild Action has appeared! It's name is {}.", action);
        return STATUS_OK;
    }

    STATUS route_server::close() {
        LOG_CTX(route_server::close, "closing server...");

        // stop server if running
        if (this->serverState == RunState::RUNNING) this->stop();

        // close the audio engine
        audioEngine.close();

        // close client manager
        clientManager.close();

        // close buffer manager
        bufferManager.close();

        // close the request channel
        requestChannel.close();

        return STATUS_OK;
    }

    STATUS route_server::start() {
        LOG_CTX(route_server::start, "starting server...");

        // set state to starting
        update_server_state(RunState::STARTING);

        // start the request channel
        STATUS ret = requestChannel.start();

        // server is now started
        update_server_state(RunState::RUNNING);

        return ret;
    }

    STATUS route_server::stop() {
        LOG_CTX(route_server::stop, "stopping server...");

        // set state to stopping
        update_server_state(RunState::STOPPING);

        // TODO alert all clients

        // stop the request channel
        STATUS ret = requestChannel.stop();

        // update state
        update_server_state(RunState::IDLE);

        return ret;
    }

    client_manager& route_server::get_client_manager() {
        return clientManager;
    }

    buffer_manager& route_server::get_buffer_manager() {
        return bufferManager;
    }

    route_engine& route_server::get_audio_engine() {
        return audioEngine;
    }

    server_info* route_server::get_server_info() {
        return  info;
    }

    STATUS route_server::update_server_state(RunState newState) {
        DBG_CTX(route_server::update_server_state, "updating server state {0}->{1}");

        serverState = newState;

        return STATUS_OK;
    }

    RunState route_server::get_state() const {
        return serverState;
    }

    graph_manager& route_server::get_graph_manager() {
        return graphManager;
    }

}