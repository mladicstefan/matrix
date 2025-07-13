//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#pragma once

#ifndef request_h
#define request_h

#include <vector>
#include "buffer.hpp"
#include<string>
#include <unordered_map>

namespace shh{

class HttpRequest{
private:
    bool parse_requestline(std::string& line);
    bool parse_headers(std::string& line);

    bool parse_body_content(shh::Buffer& buffer);
    void setup_body_parsing();
    bool should_have_body() const;
public:
    enum PARSE_STATE{
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,
    };
    HttpRequest();

    void parse_path(std::vector<std::string> files);
    //helpers for exposing private members
    const std::string& getMethod() const;
    const std::string& getPath() const;
    const std::string& getVersion() const;
    const std::string& getBody() const;
    const std::unordered_map<std::string, std::string>& getHeaders() const;

    bool isComplete() const;
    ~HttpRequest() = default;

    std::string getHeader(const std::string& name) const;
    void reset();
    bool parse(shh::Buffer &Buffer);
    bool isKeepAlive() const;
    bool shouldHaveBody() const;
private:
    std::string method_, path_, version_, body_;
    std::unordered_map<std::string, std::string> headers_;
    PARSE_STATE state_;
    int contentLength_;
    int bodyBytesRead_;
};
}

#endif /*request_h*/
