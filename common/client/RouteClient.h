#ifndef ROUTE_SUITE_ROUTECLIENT_H
#define ROUTE_SUITE_ROUTECLIENT_H

#include "pipes/ClientChannel.h"

namespace Route {

    class RouteClient {

    private:
        ClientChannel channel;
        std::string clientName = "NO_NAME";
        int ref = -1;

    public:
        RouteClient(const std::string client_name);
        ~RouteClient();

        STATUS open();
        STATUS close();

        STATUS openConfig();

        int getRef() const;

    };

}

#endif //ROUTE_SUITE_ROUTECLIENT_H
