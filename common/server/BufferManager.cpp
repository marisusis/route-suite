#include "BufferManager.h"
#include "utils.h"
#include "route_server.h"

namespace route {

    BufferManager::BufferManager(route_server* server) : server(server) {
        DBG_CTX(BufferManager::new, "");

        // set all active refs to disabled
        for (int i = 0; i < MAX_BUFFERS; i++) {
            activeBuffers[i] = false;
        }
    }

    BufferManager::~BufferManager() {
        DBG_CTX(BufferManager::~, "");
    }

    STATUS BufferManager::open() {
        DBG_CTX(BufferManager::open, "opening buffer manager...");

        // remove shm objects in case they exist
        shared_memory_object::remove(ROUTE_SHM_BUFFERS);

        // load shared memory
        shm_buffers = shared_memory_object(open_or_create, ROUTE_SHM_BUFFERS, read_write);

        shm_buffers.truncate(sizeof(route_buffer) * MAX_BUFFERS);

        // map the shared memory for buffers
        shm_buffers_region = mapped_region(shm_buffers,
                                           read_write,
                                           0,
                                           sizeof(route_buffer) * MAX_BUFFERS);

        // use the address
        buffers = static_cast<route_buffer*>(shm_buffers_region.get_address());

        return STATUS_OK;
    }

    STATUS BufferManager::close() {
        DBG_CTX(BufferManager::close, "closing buffer manager...");

        // remove our shared memory
        shared_memory_object::remove(ROUTE_SHM_BUFFERS);

        return STATUS_OK;
    }

    route_buffer* BufferManager::getBuffer(int index) {
        return &(buffers[index]);
    }

    STATUS BufferManager::freeBuffer(int buf) {

        DBG_CTX(BufferManager::freeBuffer, "freeing buffer {0}", buf);


        // check if the ref is active
        if (!activeBuffers[buf]) return STATUS_NO_REF;

        // free the reference number
        activeBuffers[buf] = false;

        // all OK
        return STATUS_OK;

    }

    STATUS BufferManager::allocateBuffer(int &buf) {
        // start at 0
        buf = 0;

        // find the next free reference number
        while (buf < MAX_BUFFERS) {

            // check if ref exists
            if (!activeBuffers[buf]) {
                // ref is free
                break;
            }

            // next ref number
            buf++;
        }

        // we got a problem if we went past the limit
        if (buf >= MAX_BUFFERS) {
            return STATUS_ERROR;
        }

        // allocate ref
        activeBuffers[buf] = true;

        DBG_CTX(BufferManager::allocateBuffer, "allocated buffer {0}", buf);

        return STATUS_OK;
    }

}