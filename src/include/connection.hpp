//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#pragma once
#ifndef connection_h
#define connection_h

#include "buffer.hpp"
#include "request.hpp"
#include "response.hpp"

namespace shh {

enum CONNECTION_STATE {
    READING,
    WRITING,
    WAITING,
    FINISH
};

class Connection{
private:
    int fd_;
    CONNECTION_STATE state_;
    shh::HttpRequest request_;
    shh::HttpResponse response_;
    shh::Buffer readBuff_;
    shh::Buffer writeBuff_;
    static const int DEFAULT_BUFF_SIZE = 4096;
    const int DEFAULT_RESPONSE_CODE = -1;
    bool keep_alive_;

public:
    explicit Connection(int client_fd);
    void handle_read();
    void handle_write(std::string web_root, std::string path);
    bool isFinished() const;
    bool isReadyToWrite() const;
    std::string getRequestPath() const;
    int getFd() const;
    ~Connection() = default;
};

}
#endif
