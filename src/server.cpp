//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "include/server.hpp"
#include "include/connection.hpp"
#include "include/epoll.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unistd.h>
#include <iostream>
#include <exception>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

const int MAX_EVENTS = SOMAXCONN;
const int TIMEOUT_MS = 200;
const int MAXFDS = SOMAXCONN;

shh::Server::Server(int domain,
                    int type,
                    int protocol,
                    u_short port,
                    in_addr_t interface,
                    uint backlog,
                    std::string path):
    srcDir_("./public"),
    path_(),
    listen_sock_(domain, type, protocol, port, interface, backlog),
    listen_fd_(listen_sock_.get_sock()),
    activeConnections_(),
    epoll_instance(MAX_EVENTS,srcDir_, path_){
        listen_sock_.bind_listen();
        // listen_fd_ = listen_sock_.get_sock();

        epoll_instance.add(listen_fd_, EPOLLIN | EPOLLET);
        std::cout << "Server started. Press Ctrl+C to stop.\n";
        std::cout << "Visit http://localhost:" << port << " in your browser\n";
}

shh::Server::~Server() {
    if (listen_fd_ > 0) {
        close(listen_fd_);
    }
}

// void shh::Server::run(){
//     try{
//         while(true){
//             epoll_instance.e_handler(listen_fd_, MAX_EVENTS, TIMEOUT_MS);
//         }
//     } catch (std::exception& e){
//         std::cerr << "Server error: " << e.what() << std::endl;
//     }
// }

void shh::Server::run() {
    try {
        while (true) {
            int n = epoll_instance.wait(listen_fd_, MAX_EVENTS ,TIMEOUT_MS);
            for (int i = 0; i < n; ++i) {
                int fd = epoll_instance.events[i].data.fd;
                uint32_t ev = epoll_instance.events[i].events;

                if (fd == listen_fd_) {
                    handle_accept();
                } else {
                    threadpool_.enqueue([this, fd, ev] {
                        handle_connection_event(fd, ev);
                    });
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << '\n';
    }
}

shh::Connection* shh::Server::get_connection(int fd){
   std::shared_lock lock(conn_mutex_);
   if (auto it = activeConnections_.find(fd); it != activeConnections_.end()){
       return it->second.get();
   }
   return nullptr;
}

// void shh::Server::remove_connection(int fd){
//     std::unique_lock lock(conn_mutex_);
//     if (auto it = activeConnections_.find(fd); it != activeConnections_.end()){
//         epoll_instance.del(fd);
//         ::close(fd);
//         activeConnections_.erase(fd);
//     }
//     //there is a race condition here, need to fix it later
// }

void shh::Server::remove_connection(int fd) {
    std::unique_lock lock(conn_mutex_);
    auto it = activeConnections_.find(fd);
    if (it == activeConnections_.end()) return;

    Connection* conn = it->second.get();
    if (!conn->try_close()) return; // Already removed or being removed

    epoll_instance.del(fd);
    ::close(fd);
    activeConnections_.erase(it);
}

void shh::Server::add_connection(int fd) {
        std::unique_lock lock(conn_mutex_);
        activeConnections_[fd] = std::make_shared<Connection>(fd);
    }

void shh::Server::handle_accept(){
    struct sockaddr_in client_addr {};
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = 0;
    while((client_fd = accept4(listen_fd_,
                                reinterpret_cast<struct sockaddr*>(&client_addr),
                                &client_addr_len,
                                SOCK_NONBLOCK)) > 0) {
        std::cout << inet_ntoa(client_addr.sin_addr) << '\n';
        std::cout << ntohs(client_addr.sin_port) << '\n';

        if (client_fd > MAXFDS){
            close(client_fd);
            continue;
        }

        {
            std::unique_lock lock(conn_mutex_);
            activeConnections_[client_fd] = std::make_shared<Connection>(client_fd);
        }

        epoll_instance.add(client_fd, EPOLLIN | EPOLLET | EPOLLONESHOT);
        }
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // no more connections right now, normal
                return;
            } else if (errno == EMFILE || errno == ENFILE) {
                std::cerr << "Reached file descriptor limit: " << std::strerror(errno) << "\n";
            } else {
                throw std::runtime_error("accept4, error: " +std::string(std::strerror(errno)));
            }
        }
}

void shh::Server::handle_connection_event(int fd, uint32_t ev) {
    Connection* conn = get_connection(fd);
    if (!conn) return;

    if (ev & (EPOLLERR | EPOLLHUP)) {
        remove_connection(fd);
        return;
    }

    if (!conn->try_lock()) {
        epoll_instance.mod(fd, EPOLLIN | EPOLLET | EPOLLONESHOT);
        return;
    }

    try {
        if (ev & EPOLLIN) {
            conn->handle_read();
            if (conn->isReadyToWrite()) {
                epoll_instance.mod(fd, EPOLLOUT | EPOLLET | EPOLLONESHOT);
            } else {
                epoll_instance.mod(fd, EPOLLIN | EPOLLET | EPOLLONESHOT);
            }
        } else if (ev & EPOLLOUT) {
            std::string path = conn->getRequestPath();
            if (path.empty() || path == "/") {
                    path ="/index.html";
            }
            std::cout << "Serving path: " << path << " from root: " << srcDir_ << std::endl;
            conn->handle_write(srcDir_, path);

            if (conn->isFinished()) {
                remove_connection(fd);
            } else {
                epoll_instance.mod(fd, EPOLLIN | EPOLLET | EPOLLONESHOT);
            }
        }
    } catch (...){
        remove_connection(fd);
    }

    conn->unlock();
}
