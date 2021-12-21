#ifndef ROUTE_SUITE_UTILS_H
#define ROUTE_SUITE_UTILS_H

#include "spdlog/spdlog.h"
#include "types.h"

#define OK(val) ((val) == STATUS_OK)

#define LOG_CTX(Context, ...) spdlog::info("["#Context"] " __VA_ARGS__)
#define TRC_CTX(Context, ...) spdlog::trace("["#Context"] " __VA_ARGS__)
#define DBG_CTX(Context, ...) spdlog::debug("["#Context"] " __VA_ARGS__)
#define ERR_CTX(Context, ...) spdlog::error("["#Context"] " __VA_ARGS__)
#define CRT_CTX(Context, ...) spdlog::critical("["#Context"] " __VA_ARGS__)
#define WRN_CTX(Context, ...) spdlog::warn("["#Context"] " __VA_ARGS__)


std::string statusToString(STATUS s);

template<typename T>
auto convert(T&& t)
{
    if constexpr (std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::string>::value)
    {
        return std::forward<T>(t).c_str();
    }
    else
    {
        return std::forward<T>(t);
    }
}

float calculateRMS(const float* values, int size);

template<typename... Args>
std::string format_string_internal(const std::string& format, Args&& ... args)
{
    const auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...) + 1;
    const auto buffer = std::make_unique<char[]>(size);

    std::snprintf(buffer.get(), size, format.c_str(), std::forward<Args>(args)...);

    return std::string(buffer.get(), buffer.get() + size - 1);
}

template<typename... Args>
std::string format_string(const std::string& format, Args&& ... args)
{
    return format_string_internal(format, convert(std::forward<Args>(args))...);
}

class Refable {

private:
    int refNum = -1;

public:
    Refable() = default;

    virtual ~Refable() = 0;

    void setRefNum(int new_refnum) {
        refNum = new_refnum;
    }

    [[nodiscard]] int getRefNum() const {
        return refNum;
    }

};

#endif //ROUTE_SUITE_UTILS_H
