//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "socket.hpp"
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/types.h>
#include "net_utils.hpp"
#include <unistd.h>

shh::Socket::Socket(int domain, int type, int protocol, u_short port, in_addr_t interface, uint backlog)
: sock(-1), connection(-1), backlog(backlog)
    {
    //set socket domain (IPV4, IPV6 ...)
    address.sin_family = domain;
    //convert port to network byte order (Big Endian VS Small Endian)
    address.sin_port = htons(port);
    // set ip to bind to, convert ip addr to network byte order
    address.sin_addr.s_addr = htonl(interface);
    //set socket fd
    sock = socket(domain,type,protocol);
    if (sock < 0) {
        throw std::runtime_error("socket() failed: " + std::string(std::strerror(errno)));
        }

    int optval = 1;
    //Set socket state (able to reconnect after restart)
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0){
        close(sock);
        throw std::runtime_error("failed to set SO_REUSEADDR " + std::string(std::strerror(errno)));
    }
    //Set TCP-KeepAlive
    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0){
        close(sock);
        throw std::runtime_error("failed to set SO_KEEPALIVE " + std::string(std::strerror(errno)));
    }
    //For use with Edge-Triggered Epoll
    if (set_non_blocking(sock) < 0){
        close(sock);
        throw std::runtime_error("non_blocking failed " + std::string(std::strerror(errno)));
    }
}

shh::Socket::~Socket(){
    if (sock >= 0) {
        close(sock);
        sock = -1; //Mark as closed
    }
    if (connection >= 0){
        close(connection);
        connection = -1;
    }
}

// watch out for dangling pointer and thread safety
struct sockaddr_in* shh::Socket::get_addr(){
    return &address;
}

int shh::Socket::get_addrsize(){
    return sizeof(address);
}

int shh::Socket::get_sock(){
    return sock;
}

int shh::Socket::get_connection(){
    return connection;
}

void shh::Socket::set_connection(int con){
    connection = con;
}

void shh::Socket::set_backlog(uint new_backlog){
    backlog = new_backlog;
}

void shh::Socket::bind_listen(){
    if (bind(sock,
        reinterpret_cast<struct sockaddr*>(&address),
        get_addrsize()) <0){
            throw::std::runtime_error((std::string("Binding failed: ") + std::strerror(errno)));
    }
    if (listen(sock, backlog) < 0){
        throw::std::runtime_error((std::string("Listening failed: ") + std::strerror(errno)));
    }
}
