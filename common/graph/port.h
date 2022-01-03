#ifndef ROUTE_SUITE_PORT_H
#define ROUTE_SUITE_PORT_H

#include <string>

namespace route {

    enum PortType {
        INPUT,
        OUTPUT
    };

    class port {

    private:
        const int ref;
        const PortType portType;
        const int clientRef;
        const int channel;
        const std::string port_name;

    public:
        port(int ref, PortType portType, int clientRef, int channel);
        port(std::string& name, int ref, PortType portType, int clientRef, int channel);

        [[nodiscard]] int get_ref() const;
        [[nodiscard]] int get_client_ref() const;
        [[nodiscard]] int get_channel() const;
        [[nodiscard]] bool is_input() const;
        [[nodiscard]] std::string get_name() const;

        bool operator==(const port &rhs) const;

        bool operator!=(const port &rhs) const;

    };

    class connection {

    private:
        const port src;
        const port dest;

    public:
        connection(const port src, const port dest);
        [[nodiscard]] const port& get_source() const;
        [[nodiscard]] const port& get_destination() const;

        bool operator==(const connection &rhs) const;

        bool operator!=(const connection &rhs) const;

    };


}


#endif //ROUTE_SUITE_PORT_H
