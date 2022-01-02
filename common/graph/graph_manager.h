#ifndef ROUTE_SUITE_GRAPH_MANAGER_H
#define ROUTE_SUITE_GRAPH_MANAGER_H


#include <map>
#include <utility>
#include <vector>
#include <types.h>
#include "constants.h"
#include "port.h"
#include <forward_list>
#include <list>
#include <optional>
#include <set>
#include <unordered_set>

namespace Route {

    class RouteServer;

    class graph_manager {


    private:
        RouteServer* server;
        bool activePorts[MAX_PORTS];
        std::map<int, port> ports;
        std::list<port> portList;
        std::list<connection> connections;

    public:
        graph_manager(RouteServer* server);
        ~graph_manager();

        STATUS allocate_port(int &id);

        STATUS free_port(int id);

        STATUS add_port(int ref, int channel, bool input);
        STATUS add_port(std::string name, int ref, int channel, bool input);
        STATUS remove_port(int ref, int channel, bool input);

        STATUS connect_ports(int a, int b);

        std::optional<const port> find_port(int client_ref, int channel, bool input);

        bool port_exists(int client_ref, int channel, bool input);
        bool port_exists(port& port);
        bool port_exists(int port_ref);

        bool connection_exists(port& a, port& b);

        std::optional<const connection> find_connection(port& src, port& dest);

        std::optional<const port> get_port(int port_ref);

        [[nodiscard]] std::list<connection> get_connections() const;

        std::list<port> getPorts() const;

        STATUS process();


    };

}

#endif //ROUTE_SUITE_GRAPH_MANAGER_H
