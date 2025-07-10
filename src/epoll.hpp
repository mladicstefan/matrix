//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#pragma once
#include <cstdint>
#include <vector>
#ifndef epoll_hpp
#define epoll_hpp

#include <sys/epoll.h>

namespace shh {

class Epoll{
public:
    // using SP_Data = std::shared_ptr<Data>;
private:
    const int MAXFDS = 100;
    std::vector<epoll_event> events;
    epoll_event* event;
    int epoll_fd;
public:
    Epoll();
    void add(int client_fd, uint32_t events);
    void mod(int client_fd, uint32_t events);
    void del(int client_fd, uint32_t events);
    void wait(int listen_fd, int MAXEVENTS, int timeout);
    void e_accept(int listen_fd, int epoll_fd);
    //void for now gonna implement return type after i finish req
    void e_handler(int listen_fd, int events_num);
    ~Epoll();
};

}

#endif
