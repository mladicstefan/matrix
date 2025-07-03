//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//
#ifndef socket_hpp
#define socket_hpp

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

namespace shh
{
class Socket
{
private:
    int connection;
    int sock;
    struct sockaddr_in address;
public:
    Socket(int domain, int type, int protocol, int port, u_long interface);
    // this is virtual in order to be able to be inherited from both the client and server
    virtual int establish_connection(int sock, struct sockaddr_in* address) = 0;
    struct sockaddr_in* get_address();
    int get_sock();
    int get_connection();
    void set_connection(int con);
    int get_addrsize();
    virtual ~Socket();
};
}
#endif  /*socket.hpp*/
