#pragma once
#include "socket.hpp"
#include "address.hpp"

namespace tcp {

    // Client: creates a connected TCP socket to the given address.
    Socket connect(const Address& addr);

}