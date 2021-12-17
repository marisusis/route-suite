#include "RouteClient.h"
#include "utils.h"
#include "constants.h"

namespace Route {

    RouteClient::RouteClient(const std::string client_name) : clientName(client_name) {
        TRC_CTX(RouteClient::new, "");

        // open client channel
        STATUS openStatus = channel.open(SERVER_NAME, clientName.c_str());

        if (openStatus != STATUS_OK) {
            CRT_CTX(RouteClient::open, "unable to establish connection with server! status={}", statusToString(openStatus));
        }

    }

    RouteClient::~RouteClient() {
        TRC_CTX(RouteClient::~, "");
    }


    STATUS RouteClient::open() {
        DBG_CTX(RouteClient::open, "opening client...");

        // send client open request to server
        ClientOpenRequest req(clientName.c_str(), _getpid());
        ClientOpenResult res;

        STATUS callStatus = channel.serverCall(&req, &res);

        if (callStatus != STATUS_OK) {
            CRT_CTX(RouteClient::open, "unable to open client!");
        }

        // set the ref
        LOG_CTX(RouteClient::open, "assigned ref {}.", res.referenceNumber);
        ref = res.referenceNumber;

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

}