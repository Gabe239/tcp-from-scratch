#include "listener.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <system_error>

Listener::Listener(int port) {
    socket_ = Socket::create(AF_INET, SOCK_STREAM, 0);

    int reuse = 1;
    setsockopt(socket_.fd(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_.fd(), (sockaddr*)&addr, sizeof(addr)) == -1)
        throw std::system_error(errno, std::generic_category(), "bind");

    if (listen(socket_.fd(), SOMAXCONN) == -1)
        throw std::system_error(errno, std::generic_category(), "listen");
}

Socket Listener::accept() {
    int client_fd = ::accept(socket_.fd(), nullptr, nullptr);
    if (client_fd == -1)
        throw std::system_error(errno, std::generic_category(), "accept");
    return Socket(client_fd);
}
