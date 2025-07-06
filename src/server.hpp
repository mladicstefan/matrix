//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//
#include "server_socket.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>

namespace shh{
class Server{
private:
    shh::ServerSocket socket;
    int epoll_fd;
public:
    Server(int port);
    void start();
};
}
