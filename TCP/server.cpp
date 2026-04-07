#include "../SocketLib/socket.hpp"
#include "../SocketLib/listener.hpp"
#include "../SocketLib/event_loop.hpp"
#include "../SocketLib/coroutine_async.hpp"
#include <iostream>
#include <unordered_map>

Task handle_client(EventLoop& loop, int fd,
                   std::unordered_map<int, Socket>& clients)
{
    char buf[256];

    while (true) {
        ssize_t n = co_await async_recv(loop, fd, buf, sizeof(buf) - 1);

        if (n <= 0) {
            std::cout << "Client disconnected fd=" << fd << "\n";
            loop.remove(fd);
            clients.erase(fd);
            co_return;
        }

        buf[n] = '\0';
        std::cout << "fd=" << fd << " says: " << buf << "\n";

        co_await async_send(loop, fd, buf, n);

        std::cout << "fd=" << fd << " reply sent\n";
    }
}

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

        handle_client(loop, cfd, clients);
    });

    std::cout << "Listening on port 6767 (coroutines)...\n";
    loop.run();

    return 0;
}
