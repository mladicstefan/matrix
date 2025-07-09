//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#pragma once
#ifndef epoll_hpp
#define epoll_hpp

#include <sys/epoll.h>

namespace shh {

class Epoll{
public:
    // using SP_Data = std::shared_ptr<Data>;
private:
    static const int MAXFDS = 100;
    static epoll_event *events;
    static int epoll_fd;
public:
    static int init();

};

}

#endif
