//
// Created by maris on 12/25/2021.
//

#ifndef ROUTE_SUITE_ROUTEENGINE_H
#define ROUTE_SUITE_ROUTEENGINE_H

#include "types.h"
#include "AudioClock.h"

namespace Route {

    class RouteServer;

    class RouteEngine {

    private:
        RouteServer* server;
        AudioClock clock;

    public:
        RouteEngine(RouteServer* server);
        ~RouteEngine();

        STATUS open();
        STATUS close();

        STATUS tick();

    };

}


#endif //ROUTE_SUITE_ROUTEENGINE_H
