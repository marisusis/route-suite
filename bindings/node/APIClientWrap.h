#ifndef ROUTE_SUITE_APICLIENTWRAP_H
#define ROUTE_SUITE_APICLIENTWRAP_H

#include <napi.h>
#include "RouteAPI.h"

using namespace RouteAPI;

class APIClientWrap : public Napi::ObjectWrap<APIClientWrap> {

private:
    RouteAPI::APIClient* client;

    // functions
    ::createClient_t createClient;
    ::openClient_t openClient;
    ::closeClient_t closeClient;
    ::getClientRef_t getRef;

public:
    explicit APIClientWrap(const Napi::CallbackInfo& info);
    Napi::Value Open(const Napi::CallbackInfo& info);
    Napi::Value Close(const Napi::CallbackInfo& info);
    Napi::Value GetRef(const Napi::CallbackInfo& info);

    static Napi::Function GetClass(Napi::Env);

};


#endif //ROUTE_SUITE_APICLIENTWRAP_H
