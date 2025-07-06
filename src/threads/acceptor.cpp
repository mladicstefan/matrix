//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "acceptor.hpp"
#include <cerrno>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>

shh::Acceptor::Acceptor(
    int listen_fd,
    struct sockaddr_in client_addr):
    listen_fd(listen_fd),
    client_addr(client_addr),
    stop_flag(false) {
}

void shh::Acceptor::run(shh::Socket& socket){
    int next_worker = 0;
    while(!stop_flag){
        socklen_t addr_len = sizeof(client_addr);
        //check if this actually gets client or just proxy.
        int client_fd = accept(socket.get_sock(),
            reinterpret_cast<struct sockaddr*>(&client_addr),
            &addr_len);
        //call add_worker here since accept is now blocking and waits for new client
        if (client_fd < 0){
            switch(errno){
            case EAGAIN:
            //no connections, yield briefly
                    // std::this_thread::sleep_for(std::chrono::milliseconds(5)); no longer need this
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

        std::lock_guard<std::mutex> lock(workers_mutex);
        //TODO EDIT ONCE U IMPLEMENT WORKERS isASSIGNED
        if (!workers.empty()){
            try{
            workers[next_worker]->add_client(client_fd);
            next_worker = (next_worker + 1)% workers.size();
            }
            catch (const std::exception& e){
            std::cerr << "Failed to assign client to worker: " << e.what() << '\n';
            // TODO: Queue client for later processing
            // pending_clients.push(client_fd);
            close(client_fd);
            }
        }
        else if (workers.empty()){
            try{
                add_worker(client_fd, workers.size());
                std::cout<<"Created worker" << workers.size()<< '\n';
            }
            catch (const std::exception& e){
                std::cerr << "Cannot create worker" << e.what()<< '\n';
                //again TODO: queue client
                close(client_fd);
            }
        }
        else {
            //not quite sure i'll handle this like this but we'll see...
            std::cerr << "No workers available, closing client\n";
            close(client_fd);
        }
    }
};
void shh::Acceptor::add_worker(int client_fd, int worker_id){
    std::lock_guard<std::mutex> lock(workers_mutex);
    workers.push_back(std::make_unique<Worker>(client_fd, worker_id));
}

void shh::Acceptor::remove_worker(){
    std::lock_guard<std::mutex> lock(workers_mutex);
    if(!workers.empty()){
        workers.back()->stop();
        workers.pop_back();
    }
    else{
        std::cerr<< "No workers available to close.";
    }
}
shh::Acceptor::~Acceptor(){
    stop_flag = true;
    std::lock_guard<std::mutex> lock(workers_mutex);
    for (auto& worker : workers){
        worker->stop(); //graceful stop
    }
    // unique_ptr destruction
    workers.clear();
}
