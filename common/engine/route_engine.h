//
// Created by maris on 12/25/2021.
//

#ifndef ROUTE_SUITE_ROUTE_ENGINE_H
#define ROUTE_SUITE_ROUTE_ENGINE_H

#include "types.h"
#include "audio_clock.h"

namespace route {

    class route_server;

    class route_engine {

    private:
        route_server* server;
        audio_clock clock;

    public:
        route_engine(route_server* server);
        ~route_engine();

        STATUS open();
        STATUS close();

        STATUS tick();

    };

}


#endif //ROUTE_SUITE_ROUTE_ENGINE_H
