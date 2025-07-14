//
// shh - C++ WebServer Test
// main execution script
//

#include "include/server.hpp"

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
    try {
        shh::Server server(AF_INET, SOCK_STREAM, 0,
                          8080, INADDR_ANY, SOMAXCONN, "../public");
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
