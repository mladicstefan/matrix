//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "epoll.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>

const int EVENTSUM = 4096;

shh::Epoll::Epoll(): epoll_fd(epoll_create1(EPOLL_CLOEXEC)), events(EVENTSUM){}
