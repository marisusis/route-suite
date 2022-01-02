//
// Created by maris on 1/1/2022.
//

#include "port.h"

#include <utility>

Route::port::port(int ref, PortType portType, int clientRef, int channel)
        : ref(ref), portType(portType), clientRef(clientRef), channel(channel) {}


Route::port::port(std::string name, int ref, Route::PortType portType, int clientRef, int channel)
        : ref(ref), portType(portType), clientRef(clientRef), channel(channel), port_name(name) {

}

bool Route::port::operator==(const Route::port &rhs) const {
    return portType == rhs.portType &&
           clientRef == rhs.clientRef &&
           channel == rhs.channel;
}

bool Route::port::operator!=(const Route::port &rhs) const {
    return !(rhs == *this);
}

int Route::port::get_ref() const {
    return ref;
}

int Route::port::get_channel() const {
    return channel;
}

int Route::port::get_client_ref() const {
    return clientRef;
}

bool Route::port::is_input() const {
    return (portType == INPUT);
}

std::string Route::port::get_name() const {
    return port_name;
}

Route::connection::connection(port src, port dest) : src(src), dest(dest) {

}

Route::port &Route::connection::get_source() const {
    return src;
}

Route::port &Route::connection::get_destination() const {
    return dest;
}

bool Route::connection::operator==(const Route::connection &rhs) const {
    return src == rhs.src &&
           dest == rhs.dest;
}

bool Route::connection::operator!=(const Route::connection &rhs) const {
    return !(rhs == *this);
}
