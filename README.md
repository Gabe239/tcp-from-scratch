# TCP Socket Library — C++

Built this to actually understand how networking works under the hood. No Boost, no ASIO, no abstractions I didn't write myself — just POSIX calls wrapped in C++.

The library is in `SocketLib/`. The server in `TCP/` got rewritten six times, each time solving a problem the previous version had.

---

## SocketLib

```
socket.hpp / socket.cpp           fd wrapper, closes itself
address.hpp / address.cpp         IPv4 address
listener.hpp / listener.cpp       bind + listen + accept
tcp_socket.hpp / tcp_socket.cpp   tcp::connect

thread_pool.hpp                   fixed threads + queue
poller.hpp                        epoll
event_loop.hpp                    reactor
async.hpp                         async recv/send with callbacks
coroutine_async.hpp               async recv/send with co_await
task.hpp                          coroutine return type
```

Sockets are move-only — one owner, no copies, destructor closes the fd. Errors throw `std::system_error` so nothing fails silently.

---

## The server rewrites

### v1 — thread per client

```
accept() → thread → handle
accept() → thread → handle
accept() → thread → handle
```

Works fine until you get enough clients. Each thread costs ~8MB of stack. At some point the OS just says no.

---

### v2 — thread pool

```
accept() ──► queue ──► worker1
                  ──► worker2
                  ──► worker3
                  ──► worker4
```

Fixed number of threads. New clients queue up instead of spawning a new thread. The problem here is workers block on `recv()` — a slow client holds a worker hostage.

---

### v3 — epoll

```
fd 3 ──┐
fd 4 ──┤
fd 5 ──┼──► epoll_wait() ──► "3 and 5 are ready"
fd 6 ──┤
fd 7 ──┘
```

One thread watches every fd. The OS wakes us up only when something actually has data. No blocking on slow clients. This is what nginx does.

---

### v4 — event loop

Wrapped the epoll loop into a class. Instead of `if (fd == listener) ... else ...` in main, you register callbacks:

```cpp
loop.on_readable(fd, []() { ... });
loop.run();
```

Same thing underneath, but now it's reusable.

---

### v5 — async callbacks

```cpp
async_recv(loop, fd, buf, [&](ssize_t n) {
    async_send(loop, fd, buf, n, [&]() {
        // done
    });
});
```

Each operation fires once and removes itself. The problem is obvious — anything more than two steps gets nested and hard to follow.

---

### v6 — coroutines

```cpp
Task handle_client(EventLoop& loop, int fd) {
    while (true) {
        ssize_t n = co_await async_recv(loop, fd, buf, sizeof(buf));
        if (n <= 0) co_return;
        co_await async_send(loop, fd, buf, n);
    }
}
```

Same behavior as v5. `co_await` suspends the coroutine without blocking the thread — the event loop keeps serving other clients while this one waits. Reads like blocking code, isn't.

---

## How it all fits together

```
clients
  │
  └── connect()
        │
        ▼
   Listener (bind + listen)
        │
        │ accept() → fd
        ▼
   EventLoop
        │
        │ epoll says fd is ready
        ▼
   handle_client() coroutine
        │
        ├── co_await recv() ──► suspend, loop runs others
        └── co_await send() ──► suspend, loop runs others
```

---

## Build

Linux, GCC 11+, C++20.

```bash
cd TCP
make
./server

# separate terminal
./client
```
