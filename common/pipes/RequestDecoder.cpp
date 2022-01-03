#include "pipes/RequestDecoder.h"
#include "server/route_server.h"
#include "utils.h"
#include "pipes/PipeRequest.h"
#include "pipes/ServerChannel.h"

namespace route {

    RequestDecoder::RequestDecoder(route_server* the_server, ChannelConnectionThread* channel_thread) : server(the_server), channelThread(channel_thread) {
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

                // add client
                server->getClientManager()->addClient(req.name, req.pid, &res.referenceNumber);

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
                DBG_CTX(RequestDecoder::handleRequest, "CLIENT_CLOSE[ref={}]", req.ref);

                // close client
                STATUS closeStatus = server->getClientManager()->closeClient(req.ref);

                if (closeStatus != STATUS_OK) {
                    CRT_CTX(RequestDecoder::handleRequest, "unable to close client [{}]! status={}", req.ref, statusToString(closeStatus));
//                    return closeStatus;
                }

                // close thread
                channelThread->close();

                return STATUS_OK;
            }

            case PipeRequest::OPEN_CONFIG: {
                LOG_CTX(RequestDecoder::handleRequest, "received OPEN_CONFIG.");

                // nothing more to do...yet
                return STATUS_OK;
            }


        }

        return STATUS_OK;
    }

}
