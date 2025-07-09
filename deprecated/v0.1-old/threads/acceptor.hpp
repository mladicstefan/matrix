//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "worker.hpp"
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include "../server_socket.hpp"
#include <mutex>

namespace shh

{
class Acceptor{
private:
int listen_fd;
std::vector<std::unique_ptr<Worker>> workers;
std::mutex workers_mutex;
struct sockaddr_in client_addr;
bool stop_flag;

public:
    Acceptor(int listen_fd, struct sockaddr_in client_addr);
    void run(shh::Socket& socket);
    void add_worker(int client_fd, int worker_id);
    void remove_worker();
    ~Acceptor();

};
}
