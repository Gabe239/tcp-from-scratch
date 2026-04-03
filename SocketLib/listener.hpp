#pragma once
#include "socket.hpp"

class Listener {
public:
    explicit Listener(int port);

    Socket accept();

    Listener(const Listener&)            = delete;
    Listener& operator=(const Listener&) = delete;
    Listener(Listener&&)                 = default;
    Listener& operator=(Listener&&)      = default;

private:
    Socket socket_;
};
