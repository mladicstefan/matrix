
//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//
#include "server_socket.hpp"
#include <netinet/in.h>
#include <sys/socket.h>

namespace shh{
class Server{
private:
    shh::ServerSocket socket;
public:
    Server(int port) : socket(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY){}

    void start(){
        // set up epoll
        socket.establish_connection(socket.get_sock(), socket.get_address());
        //add server to epoll
        // main loop
    }
};
}
