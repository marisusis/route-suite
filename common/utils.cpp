#include "utils.h"

std::string statusToString(STATUS s) {
    switch (s) {
        case STATUS_OK:
            return "STATUS_OK";
        case STATUS_ERROR:
            return "STATUS_ERROR";
        case STATUS_NO_PIPE:
            return "STATUS_NO_PIPE";
        case STATUS_NO_THREAD:
            return "STATUS_NO_THREAD";
        case STATUS_WRONG_PACKET_SIZE:
            return "STATUS_PACKET_SIZE";
        case STATUS_MUTEX_ERROR:
            return "STATUS_ERROR";
        case STATUS_BAD_REQUEST:
            return "STATUS_BAD_REQUEST";
        case STATUS_NO_TRANSACTION:
            return "STATUS_NO_TRANSACTION";
        case STATUS_NO_REF:
            return "STATUS_NO_REF";
        case STATUS_NO_BUFFER:
            return "STATUS_NO_BUFFER";
        default:
            CRT_CTX(statusToString, "bad status!");
            return "BAD_STATUS";
    }

}