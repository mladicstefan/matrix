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

shh::Server::Server(int port)
: socket(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY){
    //main epoll for reactor
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
}

void shh::Server::start(){
    int con = socket.establish_connection(socket.get_sock(), socket.get_address());
    if (con < 0){
        throw std::runtime_error("conection establish failed");
    }
    shh::Acceptor acceptor(socket.get_sock(), *socket.get_address());

    //test
    acceptor.add_worker(-1, 0);
    acceptor.add_worker(-1, 1);
    acceptor.run(socket);
}
