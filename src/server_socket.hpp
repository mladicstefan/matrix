//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//
#ifndef server_socket_hpp
#define server_socket_hpp
#include "socket.hpp"
#include <unistd.h>

namespace shh
{
class ServerSocket: public Socket{
public:
    ServerSocket(int domain,int port,int type, int protocol,u_long interface, int backlog);
    int establish_connection(int sock, struct sockaddr_in* address) override;
    int close_connection(int sock);
    int set_backlog();
};
}
#endif /* server_socket_hpp*/
