//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "request.hpp"
#include "buffer.hpp"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <regex>

shh::HttpRequest::HttpRequest(): state_(REQUEST_LINE), method_(""), path_(""), version_(""),body_(""){
    headers_.clear();
}


bool shh::HttpRequest::parse(shh::Buffer& buffer){
    if(buffer.ReadableBytes() <= 0){
        return false;
    }
    //init buffer here
   const char CRLF[] = "\r\n";
   std::string line = ""; //placeholder
   while (buffer.ReadableBytes() && state_ != FINISH){
       switch(state_){
           case REQUEST_LINE:
            parse_requestline(line);
           case HEADERS:
            parse_headers(line);
           case BODY:
            parse_body(line);
           case FINISH:
           default:
            break;
       }
   }
}

bool shh::HttpRequest::parse_requestline(std::string& line){
    std::regex pattern("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if(std::regex_match(line, subMatch, pattern )){
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = HEADERS;
        return true;
    }
    std::cerr << "Error parsing requestline" << std::strerror(errno) << '\n';
    return false;
}

bool shh::HttpRequest::parse_headers(std::string& line){
    std::regex pattern("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if(std::regex_match(line, subMatch, pattern)){
        headers_[subMatch[1]] = subMatch[2];
        return true;
    }
    std::cout << "No headers to process..." << '\n';
    state_ = BODY;
}

bool shh::HttpRequest::parse_body(std::string& line){
    //yet to be implemented
    state_ = FINISH;
    return "body";
}

void shh::HttpRequest::parse_path(std::vector<std::string> files) {
    for(auto &item: files) {
        if(item == path_) {
            path_ += ".html";
            break;
        }
    }
}
