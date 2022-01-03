#include "buffer_manager.h"
#include "utils.h"
#include "route_server.h"

namespace route {

    buffer_manager::buffer_manager(route_server* server) : server(server) {
        DBG_CTX(buffer_manager::new, "");

        // set all active refs to disabled
        for (int i = 0; i < MAX_BUFFERS; i++) {
            activeBuffers[i] = false;
        }
    }

    buffer_manager::~buffer_manager() {
        DBG_CTX(buffer_manager::~, "");
    }

    STATUS buffer_manager::open() {
        DBG_CTX(buffer_manager::open, "opening buffer manager...");

        // remove shm objects in case they exist
        shared_memory_object::remove(ROUTE_SHM_BUFFERS);

        // load shared memory
        shm_buffers = shared_memory_object(open_or_create, ROUTE_SHM_BUFFERS, read_write);

        shm_buffers.truncate(sizeof(buffer_info) * MAX_BUFFERS);

        // map the shared memory for buffers
        shm_buffers_region = mapped_region(shm_buffers,
                                           read_write,
                                           0,
                                           sizeof(buffer_info) * MAX_BUFFERS);

        // use the address
        buffers = static_cast<buffer_info*>(shm_buffers_region.get_address());

        return STATUS_OK;
    }

    STATUS buffer_manager::close() {
        DBG_CTX(buffer_manager::close, "closing buffer manager...");

        // remove our shared memory
        shared_memory_object::remove(ROUTE_SHM_BUFFERS);

        return STATUS_OK;
    }

    buffer_info* buffer_manager::get_buffer(int index) {
        return &(buffers[index]);
    }

    STATUS buffer_manager::free_buffer(int buf) {

        DBG_CTX(buffer_manager::free_buffer, "freeing buffer {0}", buf);


        // check if the ref is active
        if (!activeBuffers[buf]) return STATUS_NO_REF;

        // free the reference number
        activeBuffers[buf] = false;

        // all OK
        return STATUS_OK;

    }

    STATUS buffer_manager::allocate_buffer(int &buf) {
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

        DBG_CTX(buffer_manager::allocate_buffer, "allocated buffer {0}", buf);

        return STATUS_OK;
    }

}