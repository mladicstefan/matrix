//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//
#include "server_socket.hpp"
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

int main(){
    // std::cout << "Testing Socket"<< '\n';
    // shh::Socket soc = shh::Socket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY);

    std::cout << "Testing Server Socket"<< '\n';
    shh::ServerSocket ssoc= shh::ServerSocket(AF_INET, SOCK_STREAM, 0, 980, INADDR_ANY);
    std::cout << "Done test"<< '\n';
}
