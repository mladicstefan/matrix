//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "include/response.hpp"
#include "include/buffer.hpp"

#include <assert.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

const std::unordered_map<std::string, std::string> shh::HttpResponse::MIME_TYPE = {
    { ".html",  "text/html" },
    { ".txt",   "text/plain" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".css",   "text/css "},
    { ".ico",   "image/x-icon"},
    { ".js",    "text/javascript "},
};

const std::unordered_map<int, std::string> shh::HttpResponse::CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};

const std::unordered_map<int,std::string> shh::HttpResponse::CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

shh::HttpResponse::HttpResponse(){
    code_ = -1;
    path_ = srcDir_ = "";
    isKeepAlive_ = false;
}

void shh::HttpResponse::init(const std::string& srcDir,std::string& path,bool isKeepAlive ,int code){
    assert(srcDir != "");
    if(mmFile_) { unmapFile(); }
    code_ = code;
    isKeepAlive_ = isKeepAlive;
    path_ = path;
    srcDir_ = srcDir;
    mmFile_ = nullptr;
    mmFileStat_ = { 0 };
}

void shh::HttpResponse::ErrorResp(shh::Buffer& buffer, std::string message)
{
    std::string body;
    std::string status;
    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if(CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;
    } else {
        status = "Bad Request";
    }
    body += std::to_string(code_) + " : " + status  + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>shh</em></body></html>";
    buffer.BufferAppend("Content-length: " + std::to_string(body.size()) + "\r\n\r\n");
    buffer.BufferAppend(body);
}

std::string shh::HttpResponse::GetFileType_() {

    std::string::size_type idx = path_.find_last_of('.');
    if(idx == std::string::npos) {
        return "text/plain";
    }
    std::string suffix = path_.substr(idx);
    if(MIME_TYPE.count(suffix) == 1) {
        return MIME_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

void shh::HttpResponse::unmapFile() {
    if(mmFile_) {
        munmap(mmFile_, mmFileStat_.st_size);
        mmFile_ = nullptr;
    }
}

void shh::HttpResponse::AddState_(shh::Buffer& buffer){
    std::string status;
    if(CODE_STATUS.count(code_) == 1){
        status = CODE_STATUS.find(code_)->second;
    }else {
        code_ = 400;
        status = CODE_STATUS.find(code_)->second;
    }
    buffer.BufferAppend("HTTP/1.1 " + std::to_string(code_) + " " + status + "\r\n");
}

void shh::HttpResponse::AddHeaders_(shh::Buffer& buffer) {
    buffer.BufferAppend("Connection: ");
    if(isKeepAlive_) {
        buffer.BufferAppend("Keep-Alive\r\n");
        buffer.BufferAppend("Keep-Alive: max=6, timeout=120\r\n");
    } else{
        buffer.BufferAppend("close\r\n");
    }
    buffer.BufferAppend("Content-type: " + GetFileType_() + "\r\n");
}

void shh::HttpResponse::AddContent_(shh::Buffer& buffer){
    int srcFd = open((srcDir_ + path_).data(), O_RDONLY);
    if (srcFd < 0){
        ErrorResp(buffer,"File not found...");
        return;
    }
    //safer to use void* instead of int* and then later convert to int*
    void* mmRet = mmap(nullptr, mmFileStat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    //safe check, must be to voidptr, if passed invalid FD it will crash otherwise
    if (mmRet == MAP_FAILED) {
        ErrorResp(buffer, "Error opening file!");
        return;
    }
    // safer static cast
    // int* intmmRet = static_cast<int*>(mmRet);
    char* mmFile_ = static_cast<char*>(mmRet);
    close(srcFd);
    buffer.BufferAppend("Content-length: " + std::to_string(mmFileStat_.st_size) + "\r\n\r\n");
    buffer.BufferAppend(mmFile_, mmFileStat_.st_size);
}

void shh::HttpResponse::ProcessErrorCode_(){
    if(CODE_PATH.count(code_) == 1) {
            path_ = CODE_PATH.find(code_)->second;
            stat((srcDir_ + path_).data(), &mmFileStat_);
    }
}

void shh::HttpResponse::MakeResponse(shh::Buffer& buffer){
    //if cannot get filemetadata or S_ISDIR
    if (stat((srcDir_ + path_).data(), &mmFileStat_) < 0 || S_ISDIR(mmFileStat_.st_mode)){
        code_ = 404;
    }
    //file exists, but user lacks permissions
    else if(!(mmFileStat_.st_mode & S_IROTH)) {
        code_ = 403;
    }

    else if(code_ == -1) {
        code_ = 200;
    }
    ProcessErrorCode_();
    AddState_(buffer);
    AddHeaders_(buffer);
    AddContent_(buffer);
}
