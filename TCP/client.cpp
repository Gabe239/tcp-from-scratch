#include "../SocketLib/socket.hpp"
#include "../SocketLib/address.hpp"
#include "../SocketLib/tcp_socket.hpp"
#include <iostream>
#include <string>
#include <cstring>

int main() {
    Address addr("127.0.0.1", 6767);
    Socket sock = tcp::connect(addr);
    std::cout << "Connected to server\n";

    char buf[256];
    while (true) {
        std::string input;
        std::cout << "You: ";
        std::getline(std::cin, input);

        if (input == "quit") break;

        sock.send(input.data(), input.size());

        memset(buf, 0, sizeof(buf));
        sock.recv(buf, sizeof(buf) - 1);
        std::cout << "Server: " << buf << "\n";
    }

    return 0;
}
