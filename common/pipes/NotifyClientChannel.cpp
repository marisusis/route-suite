#include "NotifyClientChannel.h"
#include "utils.h"

namespace Route {

    NotifyClientChannel::NotifyClientChannel() {
        DBG_CTX(NotifyClientChannel::new, "");
    }

    STATUS NotifyClientChannel::open(const char* name) {
        DBG_CTX(NotifyClientChannel::open, "opening notify client channel [{}]...", name);

        STATUS connectStatus = notifyPipe.connect(PIPE_CLIENT_TAG, name, 0);

        if (connectStatus != STATUS_OK) {
            ERR_CTX(NotifyClientChannel::open, "error connecting to notify client channel [{}]!", name);
            return connectStatus;
        }

        return STATUS_OK;
    }

    STATUS NotifyClientChannel::close() {
        DBG_CTX(NotifyClientChannel::close, "closing notify client channel...");

        return STATUS_OK;
    }

}