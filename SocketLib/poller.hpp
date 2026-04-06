#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <system_error>
#include <cerrno>
#include <vector>

class Poller {
public:
    Poller() {
        _epoll_fd = epoll_create1(0);
        if (_epoll_fd == -1)
            throw std::system_error(errno, std::generic_category(), "epoll_create1");
    }

    ~Poller() {
        if (_epoll_fd != -1)
            ::close(_epoll_fd);
    }

    Poller(const Poller&)            = delete;
    Poller& operator=(const Poller&) = delete;

    // Register a fd to watch for incoming data (EPOLLIN)
    void add(int fd) {
        epoll_event ev{};
        ev.events  = EPOLLIN;   // notify when data is ready to read
        ev.data.fd = fd;        // store the fd so we know who fired

        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
            throw std::system_error(errno, std::generic_category(), "epoll_ctl ADD");
    }

    // Stop watching a fd
    void remove(int fd) {
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    }

    // Set a fd to non-blocking mode.
    // Required so recv/send never block inside the event loop.
    static void set_nonblocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1)
            throw std::system_error(errno, std::generic_category(), "fcntl F_GETFL");
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
            throw std::system_error(errno, std::generic_category(), "fcntl F_SETFL");
    }

    // Block until at least one fd is ready.
    // Returns the list of ready events.
    // timeout_ms = -1 means wait forever.
    std::vector<epoll_event> wait(int max_events = 64, int timeout_ms = -1) {
        std::vector<epoll_event> events(max_events);

        int n = epoll_wait(_epoll_fd, events.data(), max_events, timeout_ms);
        if (n == -1)
            throw std::system_error(errno, std::generic_category(), "epoll_wait");

        events.resize(n);   // trim to only the ready events
        return events;
    }

private:
    int _epoll_fd;
};
