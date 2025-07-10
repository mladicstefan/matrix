//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "epoll.hpp"
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>


const int MAXFDS = 100;

//TODO: ADD ERRNO DESCRIPTIONS

shh::Epoll::Epoll(int maxEvents): epoll_fd(epoll_create1(EPOLL_CLOEXEC)), events(maxEvents){
    assert(epoll_fd > 0 && events.size() > 0);
}

shh::Epoll::~Epoll(){
    close(epoll_fd);
}

void shh::Epoll::add(int client_fd, uint32_t events){
    struct epoll_event event;
    event.data.fd = client_fd;
    event.events = events;
    //add req logic here
    if (epoll_ctl(epoll_fd ,EPOLL_CTL_ADD, client_fd, &event) < 0){
        if (errno == ENOENT){
            std::cerr << "Warning: epoll add failed, fd not found: " << client_fd << "\n";
        } else {
            throw std::runtime_error("failed to epoll add for " + std::string(std::strerror(errno)));
        }
    }
}


void shh::Epoll::mod(int client_fd, uint32_t events){
    struct epoll_event event;
    event.data.fd = client_fd;
    event.events = events;
    //add req logic here
    if (epoll_ctl(epoll_fd ,EPOLL_CTL_MOD, client_fd, &event) < 0){
            if (errno == ENOENT){
                std::cerr << "Warning: epoll mod failed, fd not found: " << client_fd << "\n";
            } else {
                throw std::runtime_error("failed to epoll mod for " + std::string(std::strerror(errno)));
            }
    }
}


void shh::Epoll::del(int client_fd, uint32_t events){
    struct epoll_event event;
    event.data.fd = client_fd;
    event.events = events;
    //add req logic here
    if (epoll_ctl(epoll_fd ,EPOLL_CTL_DEL, client_fd, nullptr) < 0){
        if (errno == ENOENT){
            std::cerr << "Warning: epoll del failed, fd not found: " << client_fd << "\n";
        } else {
            throw std::runtime_error("failed to epoll del for " + std::string(std::strerror(errno)));
        }
    }
}

int shh::Epoll::wait(int listen_fd, int MAXEVENTS, int timeoutMs){
    return epoll_wait(epoll_fd, &events[0], static_cast<int>(events.size()), timeoutMs);
}

void shh::Epoll::e_accept(int listen_fd, int epoll_fd){
    struct sockaddr_in client_addr {};
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = 0;
    while((client_fd = accept4(listen_fd,
                                reinterpret_cast<struct sockaddr*>(&client_addr),
                                &client_addr_len,
                                SOCK_NONBLOCK)) > 0) {
        std::cout << inet_ntoa(client_addr.sin_addr) << '\n';
        std::cout << ntohs(client_addr.sin_port) << '\n';

        if (client_fd > MAXFDS){
            close(client_fd);
            continue;
        }
       //here make the req object which you pass further down epoll pipeline
    }
    if (client_fd < 0 && errno != EAGAIN && errno != EWOULDBLOCK){
        throw std::runtime_error("accept4, error: " +std::string(std::strerror(errno)));
    }
}

void shh::Epoll::e_handler(int listen_fd, int events_num){

    //bitmask for err
    uint32_t err = EPOLLERR | EPOLLHUP;

    for (int i = 0; i < events_num; i++){
        int fd = events[i].data.fd;
        uint32_t ev = events[i].events;

        if (fd == listen_fd){
            //for initial e_wait
            e_accept(listen_fd, epoll_fd);
        } else {
            if (ev & err){
                std::cerr << "epoll error on fd " << fd << '\n';
                //add timer logic after
            }
            //will need to convert fd to req obj here later...
            if (ev & EPOLLIN){
                // GET POST UEST PROCESSING
            }
            else if (ev & EPOLLOUT){
                // SERVING THE CLIENT
            }
            else if (ev & EPOLLPRI) {
                std::cerr << "EPOLLPRI received on fd " << fd << ", ignoring.\n";
            }
            else {
                std::cerr << "epoll event error " << errno << "For fd" << fd << "\n";
            }

        }

    }
}
