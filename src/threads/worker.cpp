//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "worker.hpp"
#include <sys/epoll.h>
#include <stdexcept>

shh::Worker::Worker(int client_fd, int worker_id)
    :worker_id(worker_id), stop_flag(false){

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0){
        throw std::runtime_error("worker create failed, id:", worker_id);
    }
    }
