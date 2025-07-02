//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//
#include "server_socket.hpp"

shh::ServerSocket::ServerSocket(int domain,int port,int type, int protocol,u_long interface)
: Socket(domain,type,protocol,port,interface)
{
set_connection(establish_connection(get_sock(),get_address()));
test_connection(get_connection());
}

int shh::ServerSocket::establish_connection(int sock, struct sockaddr_in* address)
{
    return bind(sock, (struct sockaddr*)address, sizeof(*address));
}
