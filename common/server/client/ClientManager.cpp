#include "ClientManager.h"
#include "utils.h"
#include "server/RouteServer.h"

namespace Route {

    ClientManager::ClientManager(RouteServer* server) : server(server) {
        DBG_CTX(ClientManager::new, "");
    }

    ClientManager::~ClientManager() {
        DBG_CTX(ClientManager::~, "");
    }

    STATUS ClientManager::open() {
        DBG_CTX(ClientManager::open, "opening ClientManager...");

        return STATUS_OK;
    }

    STATUS ClientManager::close() {
        DBG_CTX(ClientManager::close, "closing ClientManager...");

        // iterate through all
        std::map<int, Client*>::iterator it = clients.begin();

        while (it != clients.end()) {

            // close client
            DBG_CTX(ClientManager::close, "closing client [{}]...", it->first);
            it->second->close();

            // delete client
            delete it->second;

        }

        return STATUS_OK;
    }

    STATUS ClientManager::addClient(std::string clientName, const int &pid, int* ref) {
        DBG_CTX(ClientManager::addClient, "adding client [{}/{}]...", clientName, pid);

        // get a ref
        *ref = server->getNewReferenceNumber();
        DBG_CTX(ClientManager::addClient, "assigned ref [{}] to client [{}/{}]...", *ref, clientName, pid);

        // create the client
        Client* client = new Client(clientName, *ref);

        // add to the list
        clients.insert(std::make_pair(*ref, client));

        // open the client
        client->open();

        return STATUS_OK;
    }

    STATUS ClientManager::closeClient(const int ref) {
        DBG_CTX(ClientManager::closeClient, "closing client [{}]...", ref);

        // check if ref exists
        std::map<int, Client*>::iterator find = clients.find(ref);

        if (find == clients.end()) {
            // no client found
            ERR_CTX(ClientManager::closeClient, "no client found for ref [{}]!", ref);
            return STATUS_NO_REF;
        }

        // we got a client
        Client* client = find->second;

        // close the client
        client->close();

        // erase the entry
        clients.erase(find);

        // delete client
        delete client;

        return STATUS_OK;
    }

}