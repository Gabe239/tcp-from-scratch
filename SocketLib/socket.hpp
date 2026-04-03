#pragma once
#include <cstddef>
#include <sys/types.h>

class Socket {
public:
    Socket();
    explicit Socket(int fd);
    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    int fd() const;
    bool valid() const;

    void close();

    ssize_t send(const void* buf, size_t len, int flags = 0);
    ssize_t recv(void* buf, size_t len, int flags = 0);

    static Socket create(int domain, int type, int protocol = 0);

private:
    int _fd;
};