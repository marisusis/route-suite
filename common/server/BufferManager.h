#ifndef ROUTE_SUITE_BUFFERMANAGER_H
#define ROUTE_SUITE_BUFFERMANAGER_H

#include "shared/SharedStructures.h"
#include "types.h"
#include "boost/interprocess/shared_memory_object.hpp"
#include "boost/interprocess/mapped_region.hpp"

using boost::interprocess::shared_memory_object;
using boost::interprocess::mapped_region;
using boost::interprocess::create_only;
using boost::interprocess::open_or_create;
using boost::interprocess::open_only;
using boost::interprocess::read_only;
using boost::interprocess::read_write;

namespace route {

    class route_server;

    class BufferManager {

    private:
        route_server* server;
        shared_memory_object shm_buffers;
        mapped_region shm_buffers_region;
        route_buffer* buffers;
        bool activeBuffers[MAX_BUFFERS];

    public:
        BufferManager(route_server* server);
        ~BufferManager();

        STATUS open();
        STATUS close();

        route_buffer* getBuffer(int index);

        STATUS freeBuffer(int buf);
        STATUS allocateBuffer(int& buf);

    };

}


#endif //ROUTE_SUITE_BUFFERMANAGER_H
