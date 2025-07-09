//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#pragma once
#include <sys/types.h>
#ifndef socket_hpp
#define socket_hpp

#include <sys/socket.h>
#include <netinet/in.h>

namespace shh{

class Socket {
public:
    explicit Socket(int domain, int type, int protocol, u_short port ,in_addr_t interface, u_int backlog);
    struct sockaddr_in* get_addr();
    int get_addrsize();
    int get_sock();
    int get_connection();
    void set_connection(int con);
    void set_backlog(uint new_backlog);
    void bind_listen();
    ~Socket();

private:
    int sock;
    int connection;
    struct sockaddr_in address;
    int backlog = SOMAXCONN;
};

}
#endif /*socket.hpp*/
