#include "../SocketLib/socket.hpp"
#include "../SocketLib/listener.hpp"
#include "../SocketLib/event_loop.hpp"
#include "../SocketLib/async.hpp"
#include <iostream>
#include <unordered_map>

int main() {
    Listener  listener(6767);
    EventLoop loop;

    std::unordered_map<int, Socket> clients;

    EventLoop::set_nonblocking(listener.fd());

    loop.on_readable(listener.fd(), [&]() {
        Socket client = listener.accept();
        int    cfd    = client.fd();

        EventLoop::set_nonblocking(cfd);
        std::cout << "Client connected fd=" << cfd << "\n";

        clients.emplace(cfd, std::move(client));

        static char buf[256];

        async_recv(loop, cfd, buf, sizeof(buf) - 1, [&, cfd](ssize_t n) {
            if (n <= 0) {
                std::cout << "Client disconnected fd=" << cfd << "\n";
                loop.remove(cfd);
                clients.erase(cfd);
                return;
            }

            std::cout << "fd=" << cfd << " says: " << buf << "\n";

            async_send(loop, cfd, buf, n, [cfd]() {
                std::cout << "fd=" << cfd << " reply sent\n";
            });
        });
    });

    std::cout << "Listening on port 6767 (async)...\n";
    loop.run();

    return 0;
}
