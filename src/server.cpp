//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "server.hpp"
#include "server_socket.hpp"
#include <stdexcept>
#include "threads/acceptor.hpp"

shh::Server::Server(int port, int client_fd)
: socket(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY), client_fd(client_fd) {
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
}

void shh::Server::start(){
    int con = socket.establish_connection(socket.get_sock(), socket.get_address());
    if (con < 0){
        throw std::runtime_error("conection establish failed");
    }
    shh::Acceptor acceptor();
    acceptor.run(socket);
}

void shh::Server::accept_connection(){

}
