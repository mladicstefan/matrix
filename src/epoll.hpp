//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#pragma once
#ifndef epoll_hpp
#define epoll_hpp

#include <vector>
#include <cstdint>
#include <sys/epoll.h>

namespace shh {

class Epoll{
public:
    // using SP_Data = std::shared_ptr<Data>;
private:
    const int MAXFDS = 100;
    std::vector<struct epoll_event> events;
    epoll_event* event;
    int epoll_fd;
public:
    explicit Epoll(int maxEvents);
    void add(int client_fd, uint32_t events);
    void mod(int client_fd, uint32_t events);
    void del(int client_fd, uint32_t events);
    int wait(int listen_fd, int MAXEVENTS, int timeoutMs);
    void e_accept(int listen_fd, int epoll_fd);
    //void for now gonna implement return type after i finish req
    void e_handler(int listen_fd, int events_num);
    ~Epoll();
};

}

#endif
