//
// Created by maris on 9/3/2021.
//

#ifndef ROUTE_SUITE_CONSTANTS_H
#define ROUTE_SUITE_CONSTANTS_H

#define GLOBAL_NAMESPACE_PREFIX "route"

// names
#define SERVER_NAME_LENGTH 256
#define CLIENT_NAME_SIZE 128
#define SERVER_NAME "RouteD"

// Named pipes
#define PIPE_NAME_SIZE 256
#define PIPE_BUFFER_SIZE 4096
#define PIPE_NAME_DEFAULT "generic"
#define PIPE_SERVER_TAG "server"
#define PIPE_CLIENT_TAG "client"

// shared memory
#define ROUTE_SHM_INFO GLOBAL_NAMESPACE_PREFIX "_shm_" "inf"
#define ROUTE_SHM_BUFFERS GLOBAL_NAMESPACE_PREFIX "_shm_" "buffers"
#define ROUTE_SHM_CLIENTS GLOBAL_NAMESPACE_PREFIX "_shm_" "clients"

// values
#define MAX_CLIENTS 256
#define MAX_BUFFERS 512
#define MAX_BUFFER_SIZE 1024
#define MAX_CHANNELS 8
#define MAX_IN_PER_CLIENT 8
#define MAX_OUT_PER_CLIENT 8

// Status codes
//#define STATUS_OK ((STATUS) 0x1)
//#define STATUS_READ_OK STATUS_OK
//#define STATUS_WRITE_OK STATUS_OK
//#define STATUS_ERROR ((STATUS) 0x2)
//#define STATUS_NO_PIPE ((STATUS) 0x4)

#endif //ROUTE_SUITE_CONSTANTS_H
