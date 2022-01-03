//
// Created by maris on 12/25/2021.
//

#include "route_engine.h"
#include "server/route_server.h"
#include "utils.h"

route::route_engine::route_engine(route::route_server *server) : server(server), clock(this) {
    DBG_CTX(route_engine::new, "");
}


route::route_engine::~route_engine() {
    DBG_CTX(route_engine::~, "");
}

STATUS route::route_engine::open() {
    DBG_CTX(route_engine::open, "opening audio engine...");

    // open our clock
    clock.open(server->getServerInfo()->sampleRate, server->getServerInfo()->bufferSize);

    // start the clock
    clock.start();

    return STATUS_OK;
}

STATUS route::route_engine::close() {
    DBG_CTX(route_engine::close, "closing audio engine...");

    // close the clock
    clock.close();

    return STATUS_OK;
}

STATUS route::route_engine::tick() {
//    DBG_CTX(RouteEngine::tick,"");

    // process the graph
    server->get_graph_manager().process();

    return STATUS_OK;
}




