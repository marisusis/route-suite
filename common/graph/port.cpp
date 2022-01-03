//
// Created by maris on 1/1/2022.
//

#include "port.h"

#include <utility>

route::port::port(int ref, PortType portType, int clientRef, int channel)
        : ref(ref), portType(portType), clientRef(clientRef), channel(channel) {}


route::port::port(std::string& name, int ref, route::PortType portType, int clientRef, int channel)
        : ref(ref), portType(portType), clientRef(clientRef), channel(channel), port_name(std::move(name)) {

}

bool route::port::operator==(const route::port &rhs) const {
    return portType == rhs.portType &&
           clientRef == rhs.clientRef &&
           channel == rhs.channel;
}

bool route::port::operator!=(const route::port &rhs) const {
    return !(rhs == *this);
}

int route::port::get_ref() const {
    return ref;
}

int route::port::get_channel() const {
    return channel;
}

int route::port::get_client_ref() const {
    return clientRef;
}

bool route::port::is_input() const {
    return (portType == INPUT);
}

std::string route::port::get_name() const {
    return port_name;
}

route::connection::connection(const port src, const port dest) : src(src), dest(dest) {

}

const route::port &route::connection::get_source() const {
    return src;
}

const route::port &route::connection::get_destination() const {
    return dest;
}

bool route::connection::operator==(const route::connection &rhs) const {
    return src == rhs.src &&
           dest == rhs.dest;
}

bool route::connection::operator!=(const route::connection &rhs) const {
    return !(rhs == *this);
}
