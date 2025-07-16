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


class Epoll{
public:
    // using SP_Data = std::shared_ptr<Data>;
private:
    const int MAXFDS = 100;
    epoll_event* event;
    int epoll_fd;
    const std::string& srcDir_;
    std::string path_;
public:
    explicit Epoll(int maxEvents, const std::string& srcDir, std::string path);
    std::vector<struct epoll_event> events;
    void add(int client_fd, uint32_t events);
    void mod(int client_fd, uint32_t events);
    void del(int client_fd);
    int wait(int listen_fd, int MAXEVENTS, int timeoutMs);

    void e_accept(int listen_fd, int epoll_fd);
    void e_handler(int listen_fd, int MAXEVENTS, int timeoutMs);

    // ~Epoll();
};

}

#endif
