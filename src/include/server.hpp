//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "epoll.hpp"
#include "socket.hpp"
#include<string>

namespace shh{

class Server{
private:
    std::string srcDir_;
    std::string path_;
    shh::Socket listen_sock_;
    int listen_fd_;
    connMap activeConnections_;
    shh::Epoll epoll_instance;

public:
    explicit Server(int domain,
                    int type,
                    int protocol,
                    u_short port,
                    in_addr_t interface,
                    uint backlog,
                    std::string path);
    connMap GetActiveConnections();
    void run();
    ~Server();

};
}
