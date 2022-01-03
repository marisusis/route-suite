//
// Created by maris on 12/25/2021.
//

#include "RouteEngine.h"
#include "server/route_server.h"
#include "utils.h"

route::RouteEngine::RouteEngine(route::route_server *server) : server(server), clock(this) {
    DBG_CTX(RouteEngine::new, "");
}


route::RouteEngine::~RouteEngine() {
    DBG_CTX(RouteEngine::~, "");
}

STATUS route::RouteEngine::open() {
    DBG_CTX(RouteEngine::open, "opening audio engine...");

    // open our clock
    clock.open(server->getServerInfo()->sampleRate, server->getServerInfo()->bufferSize);

    // start the clock
    clock.start();

    return STATUS_OK;
}

STATUS route::RouteEngine::close() {
    DBG_CTX(RouteEngine::close, "closing audio engine...");

    // close the clock
    clock.close();

    return STATUS_OK;
}

STATUS route::RouteEngine::tick() {
//    DBG_CTX(RouteEngine::tick,"");

    // process the graph
    server->get_graph_manager().process();

    return STATUS_OK;
}




