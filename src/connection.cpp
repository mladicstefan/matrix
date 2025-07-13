//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "connection.hpp"
#include "request.hpp"
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>


int shh::Connection::getFd() const{
    return fd_;
}

bool shh::Connection::isFinished() const{
    return state_ == FINISH;
}

shh::Connection::Connection(int client_fd):
    fd_(client_fd),
    state_(READING),
    writeBuff_(DEFAULT_BUFF_SIZE),
    readBuff_(DEFAULT_BUFF_SIZE),
    request_(),
    response_(),
    keep_alive_(false)
    {
    assert(client_fd > 0);
}

void shh::Connection::handle_read(){
    size_t writable = readBuff_.WritableBytes();
    if (writable > 0){
        ssize_t bytes_read = recv(fd_, readBuff_.BeginWrite(), writable, 0);
        if (bytes_read > 0){
           readBuff_.HasWritten(bytes_read);
           if (!request_.parse(readBuff_)) {
               std::cerr << "Failed to parse HTTP request\n";
               return;
           }
           if (!request_.isComplete()) {
               std::cerr << "Incomplete HTTP request\n";
               return;
           }
           state_ = WRITING;
        }
        else if (bytes_read == 0){
            std::cout << "Client Disconnected! \n";
            state_ = FINISH;
        }
        else {
            std::cerr << "Failed to read from client or client disconnected\n";
            state_ = FINISH;
            return;
        }

    }
}

void shh::Connection::handle_write(std::string web_root, std::string path){
   bool keep_alive_ = request_.getHeader("connection") == "keep-alive";

   response_.init(web_root, path, keep_alive_, DEFAULT_RESPONSE_CODE);
   response_.MakeResponse(writeBuff_);

   const char* response_data = writeBuff_.Peek();
   size_t response_size = writeBuff_.ReadableBytes();

   ssize_t bytes_sent = send(fd_, response_data, response_size, 0);
   if (bytes_sent < 0) {
       std::cerr << "Failed to send response\n";
   } else {
       std::cout << "Response sent (" << bytes_sent << " bytes)\n";
   }
}
