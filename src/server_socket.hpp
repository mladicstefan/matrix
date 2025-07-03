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
private:

public:
    ServerSocket(int domain,int type, int protocol, int port, u_long interface);
    int establish_connection(int sock, struct sockaddr_in* address) override;
    int set_backlog();

};
}
#endif /* server_socket_hpp*/
