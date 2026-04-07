#pragma once
#include "event_loop.hpp"
#include <functional>
#include <sys/socket.h>
#include <cstring>

inline void async_recv(EventLoop& loop, int fd, void* buf, size_t len, std::function<void(ssize_t)> cb)
{
    loop.on_readable(fd, [&loop, fd, buf, len, cb]() {
        ssize_t n = ::recv(fd, buf, len, 0);

        loop.remove_readable(fd);

        cb(n);
    });
}

inline void async_send(EventLoop& loop, int fd, const void* buf, size_t len, std::function<void()> cb)
{
    ssize_t n = ::send(fd, buf, len, MSG_DONTWAIT);

    if (n == (ssize_t)len) {
        cb();
        return;
    }

    loop.on_writable(fd, [&loop, fd, buf, len, cb]() {
        ::send(fd, buf, len, 0);
        loop.remove_writable(fd);
        cb();
    });
}
