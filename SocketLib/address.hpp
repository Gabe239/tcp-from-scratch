#pragma once
#include <sys/socket.h>
#include <string>

class Address {
public:
    Address(const std::string& ip, int port);

    sockaddr*       data();
    const sockaddr* data() const;
    socklen_t       size() const;

private:
    sockaddr_storage addr_;
    socklen_t        len_;
};