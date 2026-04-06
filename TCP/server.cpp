#include "../SocketLib/socket.hpp"
#include "../SocketLib/listener.hpp"
#include "../SocketLib/poller.hpp"
#include <iostream>
#include <cstring>
#include <unordered_map>

int main() {
    Listener listener(6767);
    Poller   poller;

    Poller::set_nonblocking(listener.fd());
    poller.add(listener.fd());

    std::cout << "Listening on port 6767 (epoll)...\n";

    std::unordered_map<int, Socket> clients;

    while (true) {
        auto events = poller.wait();

        for (auto& ev : events) {
            int fd = ev.data.fd;

            if (fd == listener.fd()) {
                Socket client = listener.accept();
                int    cfd    = client.fd();

                Poller::set_nonblocking(cfd);
                poller.add(cfd);

                std::cout << "Client connected fd=" << cfd << "\n";

                clients.emplace(cfd, std::move(client));
            }
            else {
                char    buf[256];
                memset(buf, 0, sizeof(buf));

                ssize_t n = ::recv(fd, buf, sizeof(buf) - 1, 0);

                if (n <= 0) {
                    std::cout << "Client disconnected fd=" << fd << "\n";
                    poller.remove(fd);
                    clients.erase(fd);
                } else {
                    std::cout << "fd=" << fd << " says: " << buf << "\n";
                    ::send(fd, buf, n, 0);
                }
            }
        }
    }

    return 0;
}
