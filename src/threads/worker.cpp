//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "worker.hpp"
#include <iostream>
#include <string>
#include <sys/epoll.h>

shh::Worker::Worker(int client_fd, int worker_id)
    :worker_id(worker_id), stop_flag(false){

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0){
        throw std::runtime_error("worker create failed, id:"+ std::to_string(worker_id));
    }
}

void shh::Worker::add_client(int client_fd){
    std::cout<<"wowow test created client braaaah!!!!"<< "\n";
}

void shh::Worker::stop(){
    stop_flag = true;

    if(epoll_fd >= 0){
        close(epoll_fd);
        epoll_fd = -1;
    }
}

void shh::Worker::run(){
    const int MAX_EVENTS = 64;
    struct epoll_event events[MAX_EVENTS];

    while(!stop_flag){
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000); //1s timeout

        if (num_events < 0){
            if (errno == EINTR){
                std::cerr <<"epoll_wait failed" << '\n';
            }
        }

        if (num_events == 0){
            //this needs to be implemented
            cleanup_idle_connections();
        }

        for(int i = 0; i < num_events; i++){
            struct epoll_event& event = events[i];
            int client_fd = event.data.fd;

            if (event.events & EPOLLERR){
                //this needs to be implemented
                handle_error(client_fd);
            }
            else if (event.events & EPOLLHUP){
                //this needs to be implemented
                handle_client_disconnect(client_fd);
            }
            else if (event.events & EPOLLIN){
                //this needs to be implemented
                handle_client_read(client_fd);
            }
            else if (event.events & EPOLLOUT){
                //this needs to be implemented
                handle_client_send(client_fd);
            }
        }
    }

}
