#include "../SocketLib/socket.hpp"
#include "../SocketLib/listener.hpp"
#include <iostream>
#include <cstring>

int main() {
    Listener listener(9000);
    std::cout << "Listening on port 9000...\n";

    Socket client = listener.accept();
    std::cout << "Client connected\n";

    char buf[256];
    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t n = client.recv(buf, sizeof(buf) - 1);

        if (n == 0) {
            std::cout << "Client disconnected\n";
            break;
        }

        std::cout << "Received: " << buf << "\n";
        client.send(buf, n);  // echo back
    }

    return 0;
}
