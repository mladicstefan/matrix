//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "buffer.hpp"
#include <fcntl.h>
#include <string>
#include <unordered_map>
#include <sys/mman.h>

#ifndef response_hpp
#define response_hpp

namespace shh{

class HttpResponse{
private:
    int code_;
    bool isKeepAlive_;

    std::string path_;
    std::string srcDir_;

    char* mmFile_;
    //for stat sys call, to serve file.
    struct stat mmFileStat_;

    static const std::unordered_map<std::string, std::string> MIME_TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    static const std::unordered_map<int, std::string> CODE_PATH;
    std::string GetFileType_();
    void ProcessErrorCode_();
    void AddState_(shh::Buffer& buffer);
    void AddHeaders_(shh::Buffer& buffer);
    void AddContent_(shh::Buffer& buffer);

    public:
    HttpResponse();
    ~HttpResponse() = default;

    void init(const std::string& srcDir, const std::string& path, bool isKeepAlive, int code);
    void reset();

    void MakeResponse(shh::Buffer& buffer);
    void ErrorResp(shh::Buffer& buffer, std::string message);
    void unmapFile();
};
}
#endif
