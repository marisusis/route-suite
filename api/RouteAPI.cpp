//
// Created by maris on 9/7/2021.
//

#include "RouteAPI.h"

#include <utility>
#include "client/route_client.h"

namespace RouteAPI {

    APIClient::APIClient(const char *name) {
        client = new route::route_client(name);
    }

    void APIClient::open() {
        client->open();
    }

    void APIClient::close() {
        client->close();
    }

    int APIClient::getRef() {
        return client->get_ref();
    }

    EXPORT APIClient *route_create_client(const char *name) {
        return new APIClient(name);
    }

    EXPORT void route_open_client(APIClient *client) {
        client->open();
    }

    EXPORT void route_close_client(APIClient *client) {
        client->close();
    }

    EXPORT int route_get_ref(APIClient *client) {
        return client->getRef();
    }

}