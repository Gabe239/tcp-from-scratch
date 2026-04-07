#pragma once
#include "poller.hpp"
#include <functional>
#include <unordered_map>
#include <sys/epoll.h>

class EventLoop {
public:

    void on_readable(int fd, std::function<void()> cb) {
        _poller.add(fd, EPOLLIN);
        _read_handlers[fd] = std::move(cb);
    }

    void on_writable(int fd, std::function<void()> cb) {
        _poller.add(fd, EPOLLOUT);
        _write_handlers[fd] = std::move(cb);
    }

    void remove(int fd) {
        _poller.remove(fd);
        _read_handlers.erase(fd);
        _write_handlers.erase(fd);
    }

    void remove_readable(int fd) {
        _read_handlers.erase(fd);
    }

    void remove_writable(int fd) {
        _write_handlers.erase(fd);
    }

    void run() {
        while (true) {
            auto events = _poller.wait();

            for (auto& ev : events) {
                int fd = ev.data.fd;

                if (ev.events & EPOLLIN) {
                    auto it = _read_handlers.find(fd);
                    if (it != _read_handlers.end())
                        it->second();
                }

                if (ev.events & EPOLLOUT) {
                    auto it = _write_handlers.find(fd);
                    if (it != _write_handlers.end())
                        it->second();
                }
            }
        }
    }

    static void set_nonblocking(int fd) {
        Poller::set_nonblocking(fd);
    }

private:
    Poller _poller;
    std::unordered_map<int, std::function<void()>> _read_handlers;
    std::unordered_map<int, std::function<void()>> _write_handlers;
};
