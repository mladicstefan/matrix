//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//
#ifndef server_socket_hpp
#define server_socket_hpp
#include "socket.hpp"


namespace shh
{
class ServerSocket: public Socket{
public:
    ServerSocket(int domain,int port,int type, int protocol,u_long interface);
    int establish_connection(int sock, struct sockaddr_in* address);
};
}
#endif /* server_socket_hpp*/
