//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "acceptor.hpp"
#include <cerrno>
#include <iostream>
//solve this inhertiance issue, need to pass same socket obj
// shh::Acceptor::Acceptor(int listen_fd,
//     std::vector<Worker>& worker_pool,
//     struct sockaddr_in client_addr):
//         listen_fd(listen_fd),
//         workers(worker_pool),
//         stop_flag(false) {}

void shh::Acceptor::run(shh::Socket& socket){
    int next_worker = 0;

    while(!stop_flag){
        socklen_t addr_len = sizeof(client_addr);
        //check if this actually gets client or just proxy.
        int client_fd = accept4(socket.get_sock(),
            reinterpret_cast<struct sockaddr*>(&client_addr),
            &addr_len,
            SOCK_NONBLOCK
        );
        if (client_fd < 0){
            switch(errno){
            case EAGAIN:
            //no connections, yield briefly
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    break;
            case EINTR:
            //interrupted by signal, retry
                break;
            default:
            // unknown error
                std::error_code ec(errno, std::generic_category());
                std::cerr << "accept4() failed: " << ec.message() << "\n";
                break;
            }
            continue;
        }
        std::cout << "Accepted New Client fd=" << client_fd << '\n';

        //Asign worker to client
        workers[next_worker].add_client(client_fd);
        next_worker = (next_worker + 1) % workers.size();
        }
};
