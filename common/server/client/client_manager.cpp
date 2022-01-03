#include "client_manager.h"
#include "utils.h"
#include "server/route_server.h"

namespace route {

    client_manager::client_manager(route_server *server) : server(*server) {
        DBG_CTX(client_manager::new, "");

        // set all active refs to disabled
        for (int i = 0; i < MAX_CLIENTS; i++) {
            activeRefs[i] = false;
        }
    }

    client_manager::~client_manager() {
        DBG_CTX(client_manager::~, "");
    }

    STATUS client_manager::open() {
        DBG_CTX(client_manager::open, "opening ClientManager...");

        // remove shared memory in case it exists
        shared_memory_object::remove(ROUTE_SHM_CLIENTS);

        // create shared memory
        shm_clients = shared_memory_object(open_or_create, ROUTE_SHM_CLIENTS, read_write);
        shm_clients.truncate(sizeof(client_info) * MAX_CLIENTS);

        // map the memory
        shm_clients_region = mapped_region(shm_clients,
                                           read_write,
                                           0,
                                           sizeof(client_info) * MAX_CLIENTS);

        shmClients = static_cast<client_info *>(shm_clients_region.get_address());

        return STATUS_OK;
    }

    STATUS client_manager::close() {
        DBG_CTX(client_manager::close, "closing ClientManager...");

        // iterate through each client entry
        for (auto item = clients.begin(); item != clients.end(); item++) {

            // close the client
            DBG_CTX(client_manager::close, "closing client [{}]...", item->first);
            item->second->close();

            // delete the client
            delete item->second;
        }

        // remove all entries from the map
        clients.clear();

        // remove shared memory
        shared_memory_object::remove(ROUTE_SHM_CLIENTS);

        return STATUS_OK;
    }

    STATUS client_manager::add_client(std::string clientName, const int &pid, int *ref) {
        DBG_CTX(client_manager::add_client, "adding client [{}/{}]...", clientName, pid);

        // get a ref
        STATUS refGet = allocate_ref(*ref);
        if (refGet != STATUS_OK) {
            CRT_CTX(client_manager::add_client, "unable to assign refnum, limit reached!");
            return refGet;
        }

        DBG_CTX(client_manager::add_client, "assigned ref [{}] to client [{}/{}]...", *ref, clientName, pid);

        // create the client
        Client *client = new Client(clientName, *ref);

        // add to the list
        clients.insert(std::make_pair(*ref, client));

        // add the client to the shared memory
        client_info *routeClient = &(shmClients[*ref]);

        // copy name
        memcpy(routeClient->name, clientName.c_str(), sizeof(char) * 256);

        // create info
        channel_info *inInfo = new channel_info();
        channel_info *outInfo = new channel_info();

        // set active
        inInfo->active = true;
        outInfo->active = true;

        for (int i = 0; i < MAX_CHANNELS; i++) {

            int inBuf = -1;
            int outBuf = -1;

            server.get_buffer_manager().allocate_buffer(inBuf);
            server.get_buffer_manager().allocate_buffer(outBuf);

            // assign values
            routeClient->inputBufferMap[i] = inBuf;
            routeClient->outputBufferMap[i] = outBuf;

            const std::string& inName = format_string("[REF %d] Source %d @ %d", *ref, i + 1, routeClient->inputBufferMap[i]);
            const std::string& outName = format_string("[REF %d] Sink %d @ %d", *ref, i + 1, routeClient->outputBufferMap[i]);

            // set default values
            memcpy(inInfo->name, inName.c_str(), sizeof(char) * inName.length());
            memcpy(outInfo->name, outName.c_str(), sizeof(char) * outName.length());

            // copy the info to shm
            memcpy(&(routeClient->inputChannels[i]), inInfo, sizeof(channel_info));
            memcpy(&(routeClient->outputChannels[i]), outInfo, sizeof(channel_info));

            // register ports with the graph manager
            server.get_graph_manager().add_port(inInfo->name, *ref, i, true);
            server.get_graph_manager().add_port(outInfo->name, *ref, i, false);
        }

        // clean up memory
        delete inInfo;
        delete outInfo;

        // set default I/O latency
        routeClient->inputLatency = server.get_server_info()->bufferSize;
        routeClient->outputLatency = server.get_server_info()->bufferSize * 2;

        // assign mutex name
        memcpy(routeClient->clockMutexName, format_string("%s%d", ROUTE_CLOCK_MUTEX_PREFIX, *ref).c_str(), sizeof(char) * MUTEX_NAME_SIZE);

        // open the client
        client->open();

        return STATUS_OK;
    }

    STATUS client_manager::close_client(const int ref) {
        DBG_CTX(client_manager::close_client, "closing client [{}]...", ref);

        // check if ref exists
        auto find = clients.find(ref);

        if (find == clients.end()) {
            // no client found
            ERR_CTX(client_manager::close_client, "no client found for ref [{}]!", ref);
            return STATUS_NO_REF;
        }

        // we got a client
        Client *client = find->second;

        // close the client
        client->close();

        client_info shmClient = shmClients[ref];

        // free the buffers
        for (int i = 0; i < MAX_CHANNELS; i++) {
            server.get_buffer_manager().free_buffer(shmClient.inputBufferMap[i]);
            server.get_buffer_manager().free_buffer(shmClient.outputBufferMap[i]);

            // remove ports
            server.get_graph_manager().remove_port(ref, i, true);
            server.get_graph_manager().remove_port(ref, i, false);
        }

        // free its refnum
        free_ref(ref);

        // erase the entry
        clients.erase(find);

        // delete client
        delete client;

        return STATUS_OK;
    }

    STATUS client_manager::free_ref(int ref) {

        // check if the ref is active
        if (!activeRefs[ref]) return STATUS_NO_REF;

        // free the reference number
        activeRefs[ref] = false;

        // all OK
        return STATUS_OK;
    }

    STATUS client_manager::allocate_ref(int &ref) {
        // start at 0
        ref = 0;

        // find the next free reference number
        while (ref < MAX_CLIENTS) {

            // check if ref exists
            if (!activeRefs[ref]) {
                // ref is free
                break;
            }

            // next ref number
            ref++;
        }

        // we got a problem if we went past the limit
        if (ref >= MAX_CLIENTS) {
            return STATUS_ERROR;
        }

        // allocate ref
        activeRefs[ref] = true;

        return STATUS_OK;

    }

    std::map<int, Client*>* client_manager::getClients() {
        return &clients;
    }

    client_info* client_manager::get_client_info(int ref) {

        if (!activeRefs[ref]) {
            ERR_CTX(client_manager::get_client_info, "client [{}] does not exist", ref);
            return nullptr;
        }

        return &(shmClients[ref]);

    }

    buffer_info* client_manager::get_buffer_info(int ref, int channel, bool input) {

        // get the client info
        const client_info* clientInfo = get_client_info(ref);

        const int bufferIndex = input ? clientInfo->inputBufferMap[channel] : clientInfo->outputBufferMap[channel];

        return server.get_buffer_manager().get_buffer(bufferIndex);
    }


}