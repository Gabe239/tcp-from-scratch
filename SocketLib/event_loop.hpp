#pragma once
#include "poller.hpp"
#include <functional>
#include <unordered_map>

class EventLoop {
public:

    void on_readable(int fd, std::function<void()> cb) {
        _poller.add(fd);
        _handlers[fd] = std::move(cb);
    }

    void remove(int fd) {
        _poller.remove(fd);
        _handlers.erase(fd);
    }

    void run() {
        while (true) {
            auto events = _poller.wait();

            for (auto& ev : events) {
                int fd = ev.data.fd;

                auto it = _handlers.find(fd);
                if (it != _handlers.end())
                    it->second();
            }
        }
    }

    static void set_nonblocking(int fd) {
        Poller::set_nonblocking(fd);
    }

private:
    Poller _poller;
    std::unordered_map<int, std::function<void()>> _handlers;
};
