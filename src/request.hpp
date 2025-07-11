//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#pragma once
#include <vector>
#ifndef request_h
#define request_h

#include<string>
#include <unordered_map>

namespace shh{

class HttpRequest{
private:
    std::string parse_requestline();
    std::string parse_headers();
    std::string parse_body();
    void parse_path(std::vector<std::string> files);

public:
    enum PARSE_STATE{
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,
    };
    HttpRequest();
    ~HttpRequest() = default;

    bool parse(); //this needs to take in buffer later
private:
    std::string method_, path_, version_, body_;
    std::unordered_map<std::string, std::string> headers_;
    PARSE_STATE state_;
};
}

#endif /*request_h*/
