#include <napi.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "RouteAPI.h"

typedef RouteAPI::APIClient* (* createClient)(const char*);

typedef RouteAPI::APIClient* (* openClient)(RouteAPI::APIClient*);

RouteAPI::APIClient* client;

Napi::String Method(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, "world");
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "hello"),
                Napi::Function::New(env, Method));

    // load library
    HMODULE dll = LoadLibrary("RouteAPI.dll");

    std::cout << "trying..." << std::endl;

    if (dll) {
        std::cout << "going..." << std::endl;


        // get proc address
        auto createClient = reinterpret_cast<::createClient>(GetProcAddress(dll, "createClient"));
        auto openClient = reinterpret_cast<::openClient>(GetProcAddress(dll, "openClient"));

        std::cout << "create." << std::endl;


        // create the client
        client = createClient("yoy");

        std::cout << "open." << std::endl;


        // open the client
        openClient(client);

        const int sleepTime2 = 4;
        std::cout << "sleeping for " << sleepTime2 << " seconds...";
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime2));

    }

    FreeLibrary(dll);

    return exports;
}

NODE_API_MODULE(hello, Init)
