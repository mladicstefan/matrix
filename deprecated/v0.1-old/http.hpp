//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//
#include <string>
#include <unordered_map>
#include <vector>

namespace shh{
struct HTTPRequest{
    std::string method;
    std::string uri;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    bool has_header(const std::string& name) const;
    std::string get_header(const std::string& name) const;
};

class HttpParser{
public:
    bool parse(const std::string& raw_request, HTTPRequest& request);
private:
    std::vector<std::string> split_lines(const std::string& data);
    std::pair<std::string, std::string> parse_header(const std::string& line);
    bool is_valid_method(std::string& method);
};
}
