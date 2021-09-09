#include "utils.h"
#include "RouteServer.h"
#include "server/client/ClientManager.h"

namespace Route {

    RouteServer::RouteServer() : clientManager(this) {

    }

    RouteServer::~RouteServer() {

    }

    STATUS RouteServer::open() {
        LOG_CTX(RouteServer::open, "opening server...");

        // open the request channel
        requestChannel.open(this, SERVER_NAME);

        // open client manager
        clientManager.open();

        return STATUS_OK;
    }

    STATUS RouteServer::tempAction(const std::string& action) {
        LOG_CTX(RouteServer::tempAction, "A wild Action has appeared! It's name is {}.", action);
        return STATUS_OK;
    }

    int RouteServer::getNewReferenceNumber() {
        return currentReferenceNumber++;
    }

    STATUS RouteServer::close() {
        LOG_CTX(RouteServer::close, "closing server...");

        // close client manager
        clientManager.close();

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

}