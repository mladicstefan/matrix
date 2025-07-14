//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "include/epoll.hpp"
#include "include/connection.hpp"

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>

const int MAXFDS = SOMAXCONN;

shh::Epoll::Epoll(int maxEvents, connMap& connections, const std::string& srcDir, std::string path):
    epoll_fd(epoll_create1(EPOLL_CLOEXEC)),
    events(maxEvents),
    activeConnections_(connections),
    srcDir_(srcDir),
    path_(path)
{
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
        activeConnections_[client_fd] = std::make_unique<Connection>(client_fd);

        add(client_fd, EPOLLIN | EPOLLET);
    }
    if (client_fd < 0 && errno != EAGAIN && errno != EWOULDBLOCK){
        throw std::runtime_error("accept4, error: " +std::string(std::strerror(errno)));
    }
}

void shh::Epoll::e_handler(int listen_fd, int MAXEVENTS, int TIMEOUT_MS){
    int events_num = wait(listen_fd, MAXFDS, TIMEOUT_MS);

    //bitmask for err
    uint32_t err = EPOLLERR | EPOLLHUP;

    for (int i = 0; i < events_num; i++){
        int fd = events[i].data.fd;
        uint32_t ev = events[i].events;

        if (fd == listen_fd){
            //for initial e_wait
            e_accept(listen_fd, epoll_fd);
        } else {
            auto conn_it = activeConnections_.find(fd);
            if (conn_it == activeConnections_.end()) continue;
            auto& conn = conn_it -> second;

            if (ev & err){
                std::cerr << "epoll error on fd " << fd << '\n';
                activeConnections_.erase(conn_it);
                close(fd);
                continue;
            }
            if (ev & EPOLLIN){
                conn-> handle_read();
                if (conn->isReadyToWrite()){
                    mod(fd, EPOLLOUT | EPOLLET);
                }
            }
            else if (ev & EPOLLOUT){
                std::string request_path = conn->getRequestPath();
                if (request_path.empty() || request_path == "/") {
                        request_path = "/index.html";
                    }
                conn->handle_write(srcDir_,request_path);

                if(conn->isFinished()){
                    activeConnections_.erase(conn_it);
                    close(fd);
                }
                else{
                    mod(fd, EPOLLIN | EPOLLET);
                }
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
