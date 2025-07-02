//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//
#include "socket.hpp"

shh::Socket::Socket(int domain, int type, int protocol, int port, u_long interface)
{
    address.sin_family = domain;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(interface);
    sock = socket(domain, type, protocol);
};

struct sockaddr_in* shh::Socket::get_address(){
    return &address;
};

int shh::Socket::get_sock(){
    return sock;
};

int shh::Socket::get_connection(){
    return connection;
};

void shh::Socket::set_connection(int con){
    connection = con;
}
