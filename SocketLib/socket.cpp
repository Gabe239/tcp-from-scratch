#include "socket.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <system_error>

Socket::Socket() : _fd(-1) {}

Socket::Socket(int fd) : _fd(fd) {}

Socket::~Socket() {
    close();
}

Socket::Socket(Socket&& other) noexcept : _fd(other._fd) {
    other._fd = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        close();
        _fd = other._fd;
        other._fd = -1;
    }
    return *this;
}

int Socket::fd() const {
    return _fd;
}

bool Socket::valid() const {
    return _fd != -1;
}

void Socket::close() {
    if (_fd != -1) {
        ::close(_fd);
        _fd = -1;
    }
}

ssize_t Socket::send(const void* buf, size_t len, int flags) {
    ssize_t n = ::send(_fd, buf, len, flags);
    if (n == -1)
        throw std::system_error(errno, std::generic_category(), "send");
    return n;
}

ssize_t Socket::recv(void* buf, size_t len, int flags) {
    ssize_t n = ::recv(_fd, buf, len, flags);
    if (n == -1)
        throw std::system_error(errno, std::generic_category(), "recv");
    return n;
}

Socket Socket::create(int domain, int type, int protocol) {
    int fd = ::socket(domain, type, protocol);
    if (fd == -1)
        throw std::system_error(errno, std::generic_category(), "socket");
    return Socket(fd);
}