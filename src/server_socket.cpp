//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//
#include "server_socket.hpp"
#include <iostream>
#include <stdexcept>

shh::ServerSocket::ServerSocket(int domain,int type, int protocol,int port, u_long interface)
: Socket(domain,type,protocol,port,interface)
{
// set_connection(establish_connection(get_sock(),get_address()));
}

int shh::ServerSocket::establish_connection(int sock, struct sockaddr_in* address)
{
    if (bind(sock,
        reinterpret_cast<struct sockaddr*>(&address),
        get_addrsize())< 0){
        throw std::runtime_error("Binding Failed...");
    }

    //TODO: FIX THIS SHITTY CODE
    if (listen(sock, 10) < 0){
        throw std::runtime_error("Listening failed...");
    }
    std::cout << "success"<< '\n';
    return 0;
}

//setter
int shh::ServerSocket::set_backlog(){
    int backlog = 10;
    return backlog;
}
