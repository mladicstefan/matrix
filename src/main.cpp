//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "socket.hpp"
#include <iostream>

int main() {
    try {
        std::cout << "Creating socket..." << '\n';

        shh::Socket sock(AF_INET,           // domain (IPv4)
                        SOCK_STREAM,        // type (TCP)
                        0,                  // protocol (default)
                        8080,               // port
                        INADDR_ANY,         // interface (bind to all)
                        10);                // backlog

        std::cout << "Socket created successfully!" << '\n';
        std::cout << "Socket FD: " << sock.get_sock() << '\n';
        std::cout << "Port: " << ntohs(sock.get_addr()->sin_port) << '\n';

        std::cout << "Binding and listening..." << '\n';
        sock.bind_listen();
        std::cout << "Server listening on port 8080" << '\n';

        std::cout << "Test completed successfully!" << '\n';

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
