//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "server.hpp"
#include "server_socket.hpp"
#include <stdexcept>

shh::Server::Server(int port, int client_fd, int epoll_fd)
: socket(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY), client_fd(client_fd) {
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
}

void shh::Server::start(){
    int con = socket.establish_connection(socket.get_sock(), socket.get_address());
    if (con < 0){
        throw std::runtime_error("conection establish failed");
    }
}

void shh::Server::accept_connection(){
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    //check if this actually gets client or just proxy.
    client_fd = accept(socket.get_sock(),
        reinterpret_cast<struct sockaddr*>(&client_addr),
        &addr_len);

    if (client_fd < 0){
        throw std::runtime_error("failed to accept client");
    }
}

void shh::Server::epoll_events(){

}
