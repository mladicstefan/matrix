//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "request.hpp"
#include "buffer.hpp"
#include <cctype>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <regex>

shh::HttpRequest::HttpRequest(): state_(REQUEST_LINE), method_(""), path_(""), version_(""),body_(""){
    headers_.clear();
}

// Helper methods for accessing parsed data
const std::string& shh::HttpRequest::getMethod() const {
    return method_;
}

const std::string& shh::HttpRequest::getPath() const {
    return path_;
}

const std::string& shh::HttpRequest::getVersion() const {
    return version_;
}

const std::string& shh::HttpRequest::getBody() const {
    return body_;
}

const std::unordered_map<std::string, std::string>& shh::HttpRequest::getHeaders() const {
    return headers_;
}

bool shh::HttpRequest::isComplete() const{
    return state_ == FINISH;
}

bool shh::HttpRequest::parse(shh::Buffer& buffer){
    if(buffer.ReadableBytes() <= 0){
        return false;
    }
   const char CRLF[] = "\r\n";
   while (buffer.ReadableBytes() && state_ != FINISH){
       const char* lineEnd = buffer.FindCRLF();
       if (!lineEnd){
           return false;
       }
       std::string line(buffer.Peek(), lineEnd - buffer.Peek());
       buffer.RetrieveUntil(lineEnd + 2); //without \r\n
       switch(state_){
           case REQUEST_LINE:
                if (!parse_requestline(line)){
                    return false;
                }
                break;
           case HEADERS:
                parse_headers(line);
                break;
           case BODY:
                parse_body(line);
                break;
           case FINISH:
           default:
                break;
       }
   }
   return state_ == FINISH;
}

bool shh::HttpRequest::parse_requestline(std::string& line){
    std::regex pattern("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if(std::regex_match(line, subMatch, pattern )){
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        if (method_ != "GET" && method_ != "POST"){
            std::cerr << "Unsupported HTTP method: " << method_ << '\n';
            return false;
        }
        if (path_.empty() || path_[0] != '/'){
            std::cerr << "Invalid path" << path_ << '\n';
        }
        state_ = HEADERS;
        return true;
    }
    std::cerr << "Error parsing requestline" << std::strerror(errno) << '\n';
    return false;
}

bool shh::HttpRequest::parse_headers(std::string& line){
    if (line.empty()){
        state_ = BODY;
        return true;
    }

    std::regex pattern("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if(std::regex_match(line, subMatch, pattern)){
        std::string headerName = subMatch[1];
        std::string headerValue = subMatch[2];
        std::transform(headerName.begin(), headerName.end(), headerName.begin(), ::tolower);
        headers_[headerName] = headerValue;
        return true;
    }
    std::cerr << "Invalid header format: " << line << std::endl;
    return false;
}

bool shh::HttpRequest::parse_body(std::string& line){
    auto contentLength = headers_.find("content-length");
    if (contentLength != headers_.end()){
        try{
            size_t contentLenghtSize = std::stoul(contentLength->second);
            if (contentLenghtSize > 0){
                //TODO: Handle multiple line body
                body_ = line;
            }
        }catch (std::exception& e){
            std::cerr << "Invalid Content-Length: " << contentLength->second << std::endl;
            return false;
        }
    }
    state_ = FINISH;
    return true;
}

void shh::HttpRequest::parse_path(const std::vector<std::string> files) {
    for(auto &item: files) {
        if(item == path_) {
            path_ += ".html";
            break;
        }
    }
}

void shh::HttpRequest::reset() {
    state_ = REQUEST_LINE;
    method_.clear();
    path_.clear();
    version_.clear();
    body_.clear();
    headers_.clear();
}

std::string shh::HttpRequest::getHeader(const std::string& name) const {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    auto it = headers_.find(lowerName);
    return (it != headers_.end()) ? it->second : "";
}
