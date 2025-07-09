//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#pragma once
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
    int epoll_fd;
public:
    Epoll();

    ~Epoll();
};

}

#endif
