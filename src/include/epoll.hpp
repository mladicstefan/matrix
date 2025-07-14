//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#pragma once
#ifndef epoll_hpp
#define epoll_hpp


#include <memory>
#include <vector>
#include <cstdint>
#include <sys/epoll.h>
#include "connection.hpp"
namespace shh {


using connMap = std::unordered_map<int, std::unique_ptr<Connection>>;
class Epoll{
public:
    // using SP_Data = std::shared_ptr<Data>;
private:
    const int MAXFDS = 100;
    epoll_event* event;
    int epoll_fd;
    connMap& activeConnections_;
    const std::string& srcDir_;
    std::string path_;
public:
    explicit Epoll(int maxEvents, connMap& activeConnections_, const std::string& srcDir, std::string path);
    void add(int client_fd, uint32_t events);
    void mod(int client_fd, uint32_t events);
    void del(int client_fd, uint32_t events);
    int wait(int listen_fd, int MAXEVENTS, int timeoutMs);
    void e_accept(int listen_fd, int epoll_fd);
    //void for now gonna implement return type after i finish req
    void e_handler(int listen_fd, int MAXEVENTS, int timeoutMs);
    std::vector<struct epoll_event> events;
    ~Epoll();
};

}

#endif
