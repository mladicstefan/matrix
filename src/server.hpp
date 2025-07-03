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
    int client_fd = 0;
    int epoll_fd;
    // std::lock_guard<std::mutex> mut;
public:
    Server(int port, int clientfd, int epoll_fd);
    void start();
    void accept_connection();
    void epoll_events();
};
}
