//
// Created by maris on 12/25/2021.
//

#ifndef ROUTE_SUITE_ROUTEENGINE_H
#define ROUTE_SUITE_ROUTEENGINE_H

#include "types.h"
#include "AudioClock.h"

namespace route {

    class route_server;

    class RouteEngine {

    private:
        route_server* server;
        AudioClock clock;

    public:
        RouteEngine(route_server* server);
        ~RouteEngine();

        STATUS open();
        STATUS close();

        STATUS tick();

    };

}


#endif //ROUTE_SUITE_ROUTEENGINE_H
