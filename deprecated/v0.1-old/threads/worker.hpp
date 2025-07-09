//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include <thread>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

namespace shh{
class Worker{
private:
    int worker_id;
    int epoll_fd;
    std::thread thread_obj;
    bool stop_flag;

public:
    Worker(int client_fd, int worker_id);
    void add_client(int client_fd);
    void stop();
    void run();
    void cleanup_idle_connections();
    void handle_error(int client_fd);
    void handle_client_disconnect(int client_fd);
    void handle_client_send(int client_fd);
    void handle_client_read(int client_fd, epoll_data_t client_data);
};
}
