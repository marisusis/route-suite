#ifndef ROUTE_SUITE_UTILS_H
#define ROUTE_SUITE_UTILS_H

#include "spdlog/spdlog.h"

#define OK(val) ((val) == STATUS_OK)

#define LOG_CTX(Context, ...) spdlog::info("["#Context"] " __VA_ARGS__)
#define TRC_CTX(Context, ...) spdlog::trace("["#Context"] " __VA_ARGS__)
#define DBG_CTX(Context, ...) spdlog::debug("["#Context"] " __VA_ARGS__)
#define ERR_CTX(Context, ...) spdlog::error("["#Context"] " __VA_ARGS__)
#define CRT_CTX(Context, ...) spdlog::critical("["#Context"] " __VA_ARGS__)
#define WRN_CTX(Context, ...) spdlog::warn("["#Context"] " __VA_ARGS__)

#endif //ROUTE_SUITE_UTILS_H
