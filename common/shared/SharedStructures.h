//
// Created by maris on 12/20/2021.
//

#ifndef ROUTE_SUITE_SHAREDSTRUCTURES_H
#define ROUTE_SUITE_SHAREDSTRUCTURES_H

namespace Route {

    struct route_info {
        char name[256];
        char version[16];
        int sampleRate;
        int bufferSize;
    };

}

#endif //ROUTE_SUITE_SHAREDSTRUCTURES_H
