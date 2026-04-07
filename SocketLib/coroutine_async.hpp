#pragma once
#include "event_loop.hpp"
#include "task.hpp"
#include <coroutine>
#include <sys/socket.h>

struct RecvAwaitable {
    EventLoop& loop;
    int fd;
    void* buf;
    size_t len;
    ssize_t result = 0;

    bool await_ready() { return false; }

    void await_suspend(std::coroutine_handle<> handle) {
        loop.on_readable(fd, [this, handle]() {
            result = ::recv(fd, buf, len, 0);
            loop.remove_readable(fd);
            handle.resume();
        });
    }

    ssize_t await_resume() { return result; }
};

struct SendAwaitable {
    EventLoop& loop;
    int fd;
    const void* buf;
    size_t len;
    bool done = false;

    bool await_ready() {
        ssize_t n = ::send(fd, buf, len, MSG_DONTWAIT);
        done = (n == (ssize_t)len);
        return done;
    }

    void await_suspend(std::coroutine_handle<> handle) {
        loop.on_writable(fd, [this, handle]() {
            ::send(fd, buf, len, 0);
            loop.remove_writable(fd);
            handle.resume();
        });
    }

    void await_resume() {}
};

inline RecvAwaitable async_recv(EventLoop& loop, int fd, void* buf, size_t len) {
    return { loop, fd, buf, len };
}

inline SendAwaitable async_send(EventLoop& loop, int fd, const void* buf, size_t len) {
    return { loop, fd, buf, len };
}
