#ifndef ROUTE_SUITE_ROUTESERVER_H
#define ROUTE_SUITE_ROUTESERVER_H

#include <engine/RouteEngine.h>
#include <graph/graph_manager.h>
#include "pipes/ServerChannel.h"
#include "server/client/ClientManager.h"
#include "types.h"
#include "boost/interprocess/shared_memory_object.hpp"
#include "boost/interprocess/mapped_region.hpp"
#include "BufferManager.h"

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
        RouteEngine audioEngine;
        BufferManager bufferManager;
        graph_manager graphManager;
        RunState serverState = RunState::IDLE;
        int currentReferenceNumber = 1;
        shared_memory_object shm_info;
        mapped_region shm_info_region;
        route_server_info* info;

        STATUS updateServerState(RunState newState);

    public:
        RouteServer();
        ~RouteServer();

        STATUS open();
        STATUS close();

        STATUS start();
        STATUS stop();

        ClientManager* getClientManager();
        BufferManager* getBufferManager();
        RouteEngine* getAudioEngine();
        graph_manager* getGraphManager();

        RunState getState() const;

        route_server_info* getServerInfo();

        STATUS tempAction(const std::string& action);

    };

}


#endif //ROUTE_SUITE_ROUTESERVER_H
