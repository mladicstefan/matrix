//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "request.hpp"
#include <fcntl.h>
#include <string>


shh::HttpRequest::HttpRequest(): state_(REQUEST_LINE), method_(""), path_(""), version_(""),body_(""){
    headers_.clear();
}


bool shh::HttpRequest::parse(){

    //init buffer here
   const char CRLF[] = "\r\n";
   std::string line = ""; //placeholder
   while (state_ != FINISH){
       switch(state_){
           case REQUEST_LINE:
            parse_requestline();
           case HEADERS:
            parse_headers();
           case BODY:
            parse_body();
           case FINISH:
           default:
            break;
       }
   }
}

std::string shh::HttpRequest::parse_requestline(){
    state_ = HEADERS;
    return "request line";
}

std::string shh::HttpRequest::parse_headers(){
    state_ = BODY;
    return "headers";
}

std::string shh::HttpRequest::parse_body(){
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
