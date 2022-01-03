#include "graph_manager.h"
#include "utils.h"
#include <algorithm>
#include <forward_list>
#include "server/route_server.h"
#include "boost/optional.hpp"

namespace route {
    graph_manager::graph_manager(route_server* server) : server(*server) {
        LOG_CTX(graph_manager::new, "");

        // initialize graph
        for (bool &activePort: activePorts) activePort = false;

    }

    graph_manager::~graph_manager() {
        LOG_CTX(graph_manager::~, "");
    }

    STATUS graph_manager::allocate_port(int &id) {
        // start at 0
        id = 0;

        // find the next free reference number
        while (id < MAX_PORTS) {

            // check if ref exists
            if (!activePorts[id]) {
                // ref is free
                break;
            }

            // next ref number
            id++;
        }

        // we got a problem if we went past the limit
        if (id >= MAX_PORTS) {
            return STATUS_ERROR;
        }

        // allocate ref
        activePorts[id] = true;

        return STATUS_OK;

    }

    STATUS graph_manager::free_port(int id) {

        // check if the ref is active
        if (!activePorts[id]) return STATUS_NO_REF;

        // free the reference number
        activePorts[id] = false;

        // all OK
        return STATUS_OK;
    }

    STATUS graph_manager::add_port(std::string name, int ref, int channel, bool input) {
        DBG_CTX(graph_manager::add_port, "adding port for {2} channel {1} @ {0}", ref, channel, input ? "input" : "output");

        // allocate a port
        int port_ref = -1;
        const STATUS alloc_status = allocate_port(port_ref);
        if (alloc_status != STATUS_OK) {
            ERR_CTX(graph_manager::add_port, "unable to allocate port [{0}]", statusToString(alloc_status));
            return alloc_status;
        }

        // create a port
        port new_port(name, port_ref, input ? INPUT : OUTPUT, ref, channel);

        DBG_CTX(graph_manager::add_port, "adding port {} at {}", new_port.get_name(), port_ref);

        // add the port
        ports.insert({port_ref, new_port});

        return STATUS_OK;
    }

    STATUS graph_manager::add_port(int ref, int channel, bool input) {
        return add_port("no name", ref, channel, input);
    }

    STATUS graph_manager::remove_port(int ref, int channel, bool input) {
        DBG_CTX(graph_manager::remove_port, "removing port for {2} channel {1} @ {0}", ref, channel, input ? "input" : "output");

        // check if port exists
        std::optional<const port> found = find_port(ref, channel, input);
        if (!found.has_value()) return STATUS_ERROR;

        // get the port
        const port& the_port = found.value();

        // list of elements to remove
        std::forward_list<std::pair<int, int>> to_remove;


        // remove all connections that contain the port
        std::erase_if(connections, [the_port](const connection& item){
            return (item.get_source() == the_port || item.get_destination() == the_port);
        });

        const int port_ref = the_port.get_ref();

        // free the port's ref number
        free_port(port_ref);

        // remove port from the map
        ports.erase(port_ref);

        return STATUS_OK;
    }

    std::optional<const port> graph_manager::find_port(int client_ref, int channel, bool input) {

        auto findIt = std::find_if(ports.begin(), ports.end(), [=](const std::pair<const int, port>& item) {
            // get the port
            const port& p = item.second;

            // check if the parameters match
            return (p.get_client_ref() == client_ref) &&
                   (p.get_channel() == channel) &&
                   (p.is_input() == input);
        });

        if (findIt == ports.end()) return {};

        return std::optional<const port>{findIt->second};
    }

    STATUS graph_manager::connect_ports(int a, int b) {
        LOG_CTX(graph_manager::connect_ports, "attempting to connect {} --> {}", a, b);

        // get ports
        auto optPortA = get_port(a);
        auto optPortB = get_port(b);

        // check if ports exist
        if (!optPortA.has_value()) ERR_CTX(graph_manager::connect_ports, "port {} doesn't exist!", a);
        if (!optPortB.has_value()) ERR_CTX(graph_manager::connect_ports, "port {} doesn't exist!", b);

        // get ports
        const port& portA = optPortA.value();
        const port& portB = optPortB.value();

        // create connection
        connection conn(optPortA.value(), optPortB.value());

        // check if connection exists
        if (connection_exists(portA, portB)) {
            ERR_CTX(graph_manager::connect_ports, "connection [{}] --> [{}] already exists!", portA.get_name(), portB.get_name());
            return STATUS_ERROR;
        }

        // add the connection
        connections.push_back(conn);

        LOG_CTX(graph_manager::connect_ports, "[{}] --> [{}]", portA.get_name(), portB.get_name());



        return STATUS_OK;
    }

    std::list<connection> graph_manager::get_connections() const {
        return connections;
    }

    std::map<int, port> graph_manager::get_ports() const {
        return ports;
    }

    STATUS graph_manager::process() {
        // get managers
        buffer_manager& bufferManager = server.get_buffer_manager();
        client_manager& clientManager = server.get_client_manager();

        // process each connection
        for (const auto& conn : connections) {

            // get source and dest ports
            const port& srcPort = conn.get_source();
            const port& destPort = conn.get_destination();

            // get client info
            client_info* srcClient = clientManager.get_client_info(srcPort.get_client_ref());
            client_info* destClient = clientManager.get_client_info(destPort.get_client_ref());

            // get source buffer
            buffer_info* src = bufferManager.get_buffer(srcClient->outputBufferMap[srcPort.get_channel()]);
            buffer_info* dest = bufferManager.get_buffer(srcClient->inputBufferMap[srcPort.get_channel()]);

            // copy memory over
            memcpy(dest->buffer1, src->buffer1, server.get_server_info()->bufferSize * sizeof(float));
            memcpy(dest->buffer2, src->buffer2, server.get_server_info()->bufferSize * sizeof(float));

        }

        return STATUS_OK;
    }

    std::optional<const port> graph_manager::get_port(int port_ref) {

        // check if a port with the given ref exists
        if (!ports.contains(port_ref)) {

            // return nothing
            return {};
        }

        // get the port
        const port& val = ports.at(port_ref);

        // return the port
        return std::optional(val);
    }

    bool graph_manager::port_exists(int client_ref, int channel, bool input) {
        auto findIt = std::find_if(ports.begin(), ports.end(), [=](const std::pair<const int, port>& item) {
            // get the port
            const port& p = item.second;

            // check if the parameters match
            return (p.get_client_ref() == client_ref) &&
                    (p.get_channel() == channel) &&
                    (p.is_input() == input);
        });

        if (findIt == ports.end()) return false;

        return true;
    }

    bool graph_manager::port_exists(int port_ref) {
        return ports.contains(port_ref);
    }


    bool graph_manager::connection_exists(const port& a, const port& b) {
        return find_connection(a, b).has_value();
    }

    std::optional<const connection> graph_manager::find_connection(const port& src, const port& dest) {
        // search for the connection
        auto findIt = std::find_if(connections.begin(), connections.end(), [=](const connection& conn) {
            return (conn.get_source() == src) && (conn.get_destination() == dest);
        });

        if (findIt == connections.end()) {
            return {};
        }

        return std::optional(*findIt);
    }


}