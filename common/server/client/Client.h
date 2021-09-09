#ifndef ROUTE_SUITE_CLIENT_H
#define ROUTE_SUITE_CLIENT_H

#include "pipes/NotifyClientChannel.h"

namespace Route {

    class Client {

    private:
        const int ref = -1;
        std::string clientName = "NO_NAME";
        NotifyClientChannel notifyChannel;

    public:
        Client(std::string& client_name, int ref);
        ~Client();

        STATUS open();
        STATUS close();

    };

}

#endif //ROUTE_SUITE_CLIENT_H
