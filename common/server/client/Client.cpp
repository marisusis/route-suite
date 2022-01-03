#include "Client.h"
#include "utils.h"

namespace route {

    Client::Client(std::string& client_name, int ref) : ref(ref), clientName(client_name) {
        DBG_CTX(Client::new, "new client with ref [{}].", ref);
    }

    Client::~Client() {
        DBG_CTX(Client::~, "ref [{}].", ref);
    }

    STATUS Client::open() {
        LOG_CTX(Client::open, "opening client [{}]...", ref);

        // open notify channel
        notifyChannel.open(clientName.c_str());

        return STATUS_OK;
    }

    STATUS Client::close() {
        LOG_CTX(Client::close, "closing client [{}]...", ref);

        // close notify channel
        notifyChannel.close();

        return STATUS_OK;
    }

}