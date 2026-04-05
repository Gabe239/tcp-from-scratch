#include "../SocketLib/socket.hpp"
#include "../SocketLib/listener.hpp"
#include "../SocketLib/thread_pool.hpp"
#include <iostream>
#include <cstring>

void handle_client(Socket client) {
    char buf[256];

    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t n = client.recv(buf, sizeof(buf) - 1);

        if (n == 0) {
            std::cout << "Client disconnected\n";
            break;
        }

        std::cout << "Received: " << buf << "\n";
        client.send(buf, n);
    }
}

int main() {
    Listener listener(6767);
    ThreadPool pool(4);

    std::cout << "Listening on port 6767 with 4 workers...\n";

    while (true) {
        Socket client = listener.accept();
        std::cout << "Client connected\n";

        pool.submit([c = std::move(client)]() mutable {
            handle_client(std::move(c));
        });
    }

    return 0;
}
