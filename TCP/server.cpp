#include "../SocketLib/socket.hpp"
#include "../SocketLib/listener.hpp"
#include <iostream>
#include <cstring>
#include <thread> 

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
    std::cout << "Listening on port 6767...\n";

    while (true) {
        Socket client = listener.accept();
        std::cout << "Client connected\n";

        std::thread(handle_client, std::move(client)).detach();
    }

    return 0;
}
