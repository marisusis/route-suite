#include "pipes/RequestDecoder.h"
#include "server/RouteServer.h"
#include "utils.h"
#include "pipes/PipeRequest.h"
#include "pipes/ServerChannel.h"

namespace Route {

    RequestDecoder::RequestDecoder(RouteServer* the_server, ChannelConnectionThread* channel_thread) : server(the_server), channelThread(channel_thread) {
        DBG_CTX(RequestDecoder::new, "");
    }

    RequestDecoder::~RequestDecoder() {
        DBG_CTX(RequestDecoder::~, "");
    }

    STATUS RequestDecoder::handleRequest(PipeClient* pipe, int type) {
        DBG_CTX(RequestDecoder::handleRequest, "handling request type={}", type);

        switch (type) {

            case PipeRequest::CLIENT_OPEN: {
                LOG_CTX(RequestDecoder::handleRequest, "received CLIENT_OPEN.");
                ClientOpenRequest req;
                ClientOpenResult res;

                // read request
                req.read(pipe);

                // get a reference number for our client
                res.referenceNumber = server->getNewReferenceNumber();

                // write result
                res.write(pipe);

                DBG_CTX(RequestDecoder::handleRequest, "CLIENT_OPEN[name={},pid={}]", req.name, req.pid);
                return STATUS_OK;
            }

            case PipeRequest::CLIENT_CLOSE: {
                LOG_CTX(RequestDecoder::handleRequest, "received CLIENT_CLOSE.");

                // only the request is needed
                ClientCloseRequest req;

                // read request
                req.read(pipe);
                DBG_CTX(RequestDecoder::handleRequest, "CLIENT_CLOSE[name={},pid={}]", req.name, req.pid);

                // close thread
                channelThread->close();

                return STATUS_OK;

            }


        }

        return STATUS_OK;
    }

}
