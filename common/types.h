#ifndef ROUTE_SUITE_TYPES_H
#define ROUTE_SUITE_TYPES_H

#include <string>

enum RunState {
    RUNNING,
    IDLE,
    STARTING,
    STOPPING
};

typedef enum STATUS_T {

    // All is ok.
    STATUS_OK,

    // Something went wrong
    STATUS_ERROR,
    STATUS_NO_PIPE,
    STATUS_NO_THREAD,
    STATUS_WRONG_PACKET_SIZE,
    STATUS_MUTEX_ERROR,
    STATUS_BAD_REQUEST,
    STATUS_NO_TRANSACTION,
    STATUS_NO_REF,
    STATUS_NO_BUFFER,
    STATUS_BREAK,
    STATUS_PORT_EXISTS

} STATUS;


#endif //ROUTE_SUITE_TYPES_H
