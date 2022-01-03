#include "APIClientWrap.h"
#include <windows.h>

APIClientWrap::APIClientWrap(const Napi::CallbackInfo &info) : ObjectWrap(info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
                .ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to introduce yourself to greet")
                .ThrowAsJavaScriptException();
        return;
    }

    // load library
    HMODULE dll = LoadLibrary("RouteAPI.dll");

    // load functions
    createClient = reinterpret_cast<::createClient_t>(GetProcAddress(dll, "route_create_client"));
    openClient = reinterpret_cast<::openClient_t>(GetProcAddress(dll, "route_open_client"));
    closeClient = reinterpret_cast<::closeClient_t>(GetProcAddress(dll, "route_close_client"));
    getRef = reinterpret_cast<::getClientRef_t>(GetProcAddress(dll, "route_get_ref"));

    // create client instance
    client = createClient(info[0].As<Napi::String>().Utf8Value().c_str());

}

Napi::Value APIClientWrap::Open(const Napi::CallbackInfo &info) {

    // open client
    openClient(client);

    return Napi::Value();
}

Napi::Value APIClientWrap::Close(const Napi::CallbackInfo &info) {
    // close client
    closeClient(client);

    return Napi::Value();
}

Napi::Value APIClientWrap::GetRef(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, getRef(client));
}

Napi::Function APIClientWrap::GetClass(Napi::Env env) {
    return DefineClass(
            env,
            "APIClient",
            {
                    APIClientWrap::InstanceMethod("open", &APIClientWrap::Open),
                    APIClientWrap::InstanceMethod("close", &APIClientWrap::Close),
                    APIClientWrap::InstanceMethod("get_ref", &APIClientWrap::GetRef)
            });
}



Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New(env, "APIClientWrap");
    exports.Set(name, APIClientWrap::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)