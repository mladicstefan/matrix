//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "worker.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include "../server_socket.hpp"

namespace shh

{
class Acceptor{
private:
int listen_fd;
std::vector<Worker>& workers;
std::thread thread_obj;
struct sockaddr_in client_addr;
bool stop_flag;

public:
    Acceptor();
    // Acceptor(int listen_fd, std::vector<Worker>& worker_pool, struct sockaddr_in client_addr);
    void run(shh::Socket& socket);
    void start();
    void stop();
};
}
