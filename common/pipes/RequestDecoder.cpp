#include "RequestDecoder.h"
#include "server/RouteServer.h"
#include "utils.h"
#include "PipeRequest.h"

namespace Route {

    RequestDecoder::RequestDecoder(RouteServer* the_server) : server(the_server) {
        DBG_CTX(RequestDecoder::new, "");
    }

    RequestDecoder::~RequestDecoder() {
        DBG_CTX(RequestDecoder::~, "");
    }

    STATUS RequestDecoder::handleRequest(PipeClient* pipe, int type) {
        DBG_CTX(RequestDecoder::handleRequest, "handling request type={}", type);

        switch (type) {

            case PipeRequest::CLIENT_OPEN:
                DBG_CTX(RequestDecoder::handleRequest, "received CLIENT_OPEN.");
                ClientOpenRequest req;
                PipeResult res;

                // read request
                req.read(pipe);

                DBG_CTX(RequestDecoder::handleRequest, "CLIENT_OPEN[name={},pid={}]", req.name, req.pid);
                return STATUS_OK;

        }

        return STATUS_NO_THREAD;
    }

}
