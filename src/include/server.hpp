//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "connection.hpp"
#include "epoll.hpp"
#include "socket.hpp"
#include "threadpool.hpp"
#include <cstdint>
#include <shared_mutex>
#include<string>

namespace shh{

using connMap = std::unordered_map<int, std::shared_ptr<Connection>>;

class Server{
private:
    std::string srcDir_;
    std::string path_;
    shh::Socket listen_sock_;
    int listen_fd_;

    connMap activeConnections_;
    mutable std::shared_mutex conn_mutex_;
    shh::ThreadPool threadpool_;

    shh::Epoll epoll_instance;

public:
    explicit Server(int domain,
                    int type,
                    int protocol,
                    u_short port,
                    in_addr_t interface,
                    uint backlog,
                    std::string path);
    std::shared_ptr<shh::Connection> get_connection(int fd);
    void add_connection(int fd);
    void remove_connection(int fd);
    void handle_accept();
    void run();
    void handle_connection_event(int fd, uint32_t ev);
    ~Server();

};
}
