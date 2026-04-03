#include "address.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>

Address::Address(const std::string& ip, int port) {
    memset(&addr_, 0, sizeof(addr_));

    auto* v4 = reinterpret_cast<sockaddr_in*>(&addr_);
    v4->sin_family = AF_INET;
    v4->sin_port   = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &v4->sin_addr) != 1)
        throw std::invalid_argument("invalid IPv4 address: " + ip);

    len_ = sizeof(sockaddr_in);
}

sockaddr* Address::data() {
    return reinterpret_cast<sockaddr*>(&addr_);
}

const sockaddr* Address::data() const {
    return reinterpret_cast<const sockaddr*>(&addr_);
}

socklen_t Address::size() const {
    return len_;
}