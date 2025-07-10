//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include<string>
#include <unordered_map>

namespace shh{

class HttpRequest{
private:
    std::string parse_requestline();
    std::string parse_header();
    std::string parse_body();
    std::string parse_path;

    std::string method_, path_, version_, body_;
    std::unordered_map<std::string, std::string> header_;
public:
    enum PARSE_STATE{
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,
    };

    HttpRequest();
    ~HttpRequest() = default;

};
}
