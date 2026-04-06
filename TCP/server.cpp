#include "../SocketLib/socket.hpp"
#include "../SocketLib/listener.hpp"
#include "../SocketLib/poller.hpp"
#include <iostream>
#include <cstring>
#include <unordered_map>

int main() {
    Listener listener(6767);
    Poller   poller;

    // Set the listening socket to non-blocking and register it with epoll.
    // From now on epoll_wait will tell us when a new client is knocking.
    Poller::set_nonblocking(listener.fd());
    poller.add(listener.fd());

    std::cout << "Listening on port 6767 (epoll)...\n";

    // Keep all connected client sockets alive in a map: fd → Socket
    std::unordered_map<int, Socket> clients;

    while (true) {
        // Block here until at least one fd is ready.
        // Returns only the fds that actually have data — no wasted work.
        auto events = poller.wait();

        for (auto& ev : events) {
            int fd = ev.data.fd;

            // ── New client connecting ─────────────────────────────────────
            if (fd == listener.fd()) {
                Socket client = listener.accept();
                int    cfd    = client.fd();

                Poller::set_nonblocking(cfd);   // must be non-blocking
                poller.add(cfd);                // watch this client too

                std::cout << "Client connected fd=" << cfd << "\n";

                // store the socket so it stays alive
                clients.emplace(cfd, std::move(client));
            }
            // ── Existing client sent data ─────────────────────────────────
            else {
                char    buf[256];
                memset(buf, 0, sizeof(buf));

                ssize_t n = ::recv(fd, buf, sizeof(buf) - 1, 0);

                if (n <= 0) {
                    // 0 = client disconnected, -1 = error
                    std::cout << "Client disconnected fd=" << fd << "\n";
                    poller.remove(fd);
                    clients.erase(fd);  // Socket destructor closes the fd
                } else {
                    std::cout << "fd=" << fd << " says: " << buf << "\n";
                    ::send(fd, buf, n, 0);  // echo back
                }
            }
        }
    }

    return 0;
}
