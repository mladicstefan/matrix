//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "include/server.hpp"
#include "include/epoll.hpp"

#include <unistd.h>
#include <iostream>
#include <exception>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

const int MAX_EVENTS = SOMAXCONN;
const int TIMEOUT_MS = 400;


shh::Server::Server(int domain,
                    int type,
                    int protocol,
                    u_short port,
                    in_addr_t interface,
                    uint backlog,
                    std::string path):
    srcDir_("../public"),
    path_(),
    listen_sock_(domain, type, protocol, port, interface, backlog),
    listen_fd_(listen_sock_.get_sock()),
    activeConnections_(),
    epoll_instance(MAX_EVENTS, activeConnections_, srcDir_, path_){
        listen_sock_.bind_listen();
        // listen_fd_ = listen_sock_.get_sock();

        epoll_instance.add(listen_fd_, EPOLLIN | EPOLLET);
        std::cout << "Server started. Press Ctrl+C to stop.\n";
        std::cout << "Visit http://localhost:" << port << " in your browser\n";
}

shh::Server::~Server() {
    if (listen_fd_ > 0) {
        close(listen_fd_);
    }
}
void shh::Server::run(){
    try{
        while(true){
            epoll_instance.e_handler(listen_fd_, MAX_EVENTS, TIMEOUT_MS);
        }
    } catch (std::exception& e){
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}
