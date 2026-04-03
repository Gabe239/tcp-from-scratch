#include "tcp_socket.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <system_error>

namespace tcp {

Socket connect(const Address& addr) {
    Socket sock = Socket::create(AF_INET, SOCK_STREAM, 0);
    if (::connect(sock.fd(), addr.data(), addr.size()) == -1)
        throw std::system_error(errno, std::generic_category(), "connect");
    return sock;
}

}