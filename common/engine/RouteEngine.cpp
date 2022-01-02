//
// Created by maris on 12/25/2021.
//

#include "RouteEngine.h"
#include "server/RouteServer.h"
#include "utils.h"

Route::RouteEngine::RouteEngine(Route::RouteServer *server) : server(server), clock(this) {
    DBG_CTX(RouteEngine::new, "");
}


Route::RouteEngine::~RouteEngine() {
    DBG_CTX(RouteEngine::~, "");
}

STATUS Route::RouteEngine::open() {
    DBG_CTX(RouteEngine::open, "opening audio engine...");

    // open our clock
    clock.open(server->getServerInfo()->sampleRate, server->getServerInfo()->bufferSize);

    // start the clock
    clock.start();

    return STATUS_OK;
}

STATUS Route::RouteEngine::close() {
    DBG_CTX(RouteEngine::close, "closing audio engine...");

    // close the clock
    clock.close();

    return STATUS_OK;
}

STATUS Route::RouteEngine::tick() {
//    DBG_CTX(RouteEngine::tick,"");

    // process the graph
    server->getGraphManager()->process();

    return STATUS_OK;
}




