#ifndef ROUTE_SUITE_BUFFER_MANAGER_H
#define ROUTE_SUITE_BUFFER_MANAGER_H

#include "shared/shared_structures.h"
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

    class buffer_manager {

    private:
        route_server* server;
        shared_memory_object shm_buffers;
        mapped_region shm_buffers_region;
        buffer_info* buffers;
        bool activeBuffers[MAX_BUFFERS];

    public:
        explicit buffer_manager(route_server* server);
        ~buffer_manager();

        STATUS open();
        STATUS close();

        buffer_info* get_buffer(int index);

        STATUS free_buffer(int buf);
        STATUS allocate_buffer(int& buf);

    };

}


#endif //ROUTE_SUITE_BUFFER_MANAGER_H
