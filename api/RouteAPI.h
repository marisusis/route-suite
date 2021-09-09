#ifndef ROUTE_SUITE_ROUTEAPI_H
#define ROUTE_SUITE_ROUTEAPI_H

#include <string>

#define EXPORT  __declspec(dllexport)

namespace Route {
    class RouteClient;
}

namespace RouteAPI {

    class APIClient {

    private:
        Route::RouteClient* client;

    public:
        APIClient(const char* name);
        void open();
        void close();
        int getRef();

    };

    EXPORT APIClient* route_create_client(const char* name);
    EXPORT void route_open_client(APIClient* client);
    EXPORT void route_close_client(APIClient* client);
    EXPORT int route_get_ref(APIClient* client);

}

typedef RouteAPI::APIClient* (* createClient_t)(const char*);
typedef RouteAPI::APIClient* (* openClient_t)(RouteAPI::APIClient*);
typedef RouteAPI::APIClient* (* closeClient_t)(RouteAPI::APIClient*);
typedef int (* getClientRef_t)(RouteAPI::APIClient*);

#endif //ROUTE_SUITE_ROUTEAPI_H
