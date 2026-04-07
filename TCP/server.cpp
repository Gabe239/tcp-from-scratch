#include "../SocketLib/socket.hpp"
#include "../SocketLib/listener.hpp"
#include "../SocketLib/event_loop.hpp"
#include <iostream>
#include <cstring>
#include <unordered_map>

int main() {
    Listener listener(6767);
    EventLoop loop;

    std::unordered_map<int, Socket> clients;

    EventLoop::set_nonblocking(listener.fd());

    loop.on_readable(listener.fd(), [&]() {
        Socket client = listener.accept();
        int    cfd    = client.fd();

        EventLoop::set_nonblocking(cfd);

        std::cout << "Client connected fd=" << cfd << "\n";

        loop.on_readable(cfd, [cfd, &loop, &clients]() {
            char buf[256];
            memset(buf, 0, sizeof(buf));

            ssize_t n = ::recv(cfd, buf, sizeof(buf) - 1, 0);

            if (n <= 0) {
                std::cout << "Client disconnected fd=" << cfd << "\n";
                loop.remove(cfd);
                clients.erase(cfd);
            } else {
                std::cout << "fd=" << cfd << " says: " << buf << "\n";
                ::send(cfd, buf, n, 0);
            }
        });

        clients.emplace(cfd, std::move(client));
    });

    std::cout << "Listening on port 6767 (event loop)...\n";
    loop.run();

    return 0;
}
