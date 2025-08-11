//
// shh - C++ WebServer Test
// main execution script
//

#include "include/server.hpp"

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char** argv) {

    if (argc != 3){
        std::cerr<< "Usage: server <port> <source directory>\n";
        return 1;
    }

    try {
        int port = std::stoi(argv[1]);
        std::string srcDir = argv[2];

	// when running outside of container do this
	//shh::Server server(AF_INET, SOCK_STREAM, 0,
			   //port, inet_addr("127.0.0.1"), SOMAXCONN,srcDir);

        shh::Server server(AF_INET, SOCK_STREAM, 0,
                          port, INADDR_ANY , SOMAXCONN,srcDir);

        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
