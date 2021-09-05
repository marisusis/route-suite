#include "RequestDecoder.h"
#include "server/RouteServer.h"
#include "utils.h"

namespace Route {

    RequestDecoder::RequestDecoder(RouteServer* the_server) : server(the_server) {
        DBG_CTX(RequestDecoder::new, "");
    }

    RequestDecoder::~RequestDecoder() {
        DBG_CTX(RequestDecoder::~, "");
    }

    STATUS RequestDecoder::handleRequest(PipeClient* pipe, int type) {
        DBG_CTX(RequestDecoder::handleRequest, "handling request type={}", type);

        return STATUS_NO_THREAD;
    }

}
