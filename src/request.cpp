//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "include/request.hpp"
#include "include/buffer.hpp"

#include <cctype>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <regex>
#include <algorithm>

shh::HttpRequest::HttpRequest(): state_(REQUEST_LINE), method_(""), path_(""), version_(""), body_(""), contentLength_(0), bodyBytesRead_(0) {
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

bool shh::HttpRequest::isComplete() const {
    return state_ == FINISH;
}

bool shh::HttpRequest::parse(shh::Buffer& buffer) {
    if (buffer.ReadableBytes() <= 0) {
        return false;
    }

    while (buffer.ReadableBytes() && state_ != FINISH) {
        if (state_ == BODY) {
            return parse_body_content(buffer);
        }

        const char* lineEnd = buffer.FindCRLF();
        if (!lineEnd) {
            return false;
        }

        std::string line(buffer.Peek(), lineEnd - buffer.Peek());
        buffer.RetrieveUntil(lineEnd + 2); // Skip \r\n

        switch (state_) {
            case REQUEST_LINE:
                if (!parse_requestline(line)) {
                    return false;
                }
                break;
            case HEADERS:
                if (!parse_headers(line)) {
                    return false;
                }
                break;
            case BODY:
                // This case is now handled above
                break;
            case FINISH:
            default:
                break;
        }
    }

    return state_ == FINISH;
}

bool shh::HttpRequest::parse_requestline(std::string& line) {
    std::regex pattern("^([A-Z]+) ([^ ]*) HTTP/([0-9]\\.[0-9])$");
    std::smatch subMatch;

    if (std::regex_match(line, subMatch, pattern)) {
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];

        // Validate method
        if (method_ != "GET") {
            std::cerr << "Unsupported HTTP method: " << method_ << '\n';
            return false;
        }

        // Validate path
        if (path_.empty() || path_[0] != '/') {
            std::cerr << "Invalid path: " << path_ << '\n';
            return false;
        }

        state_ = HEADERS;
        return true;
    }

    std::cerr << "Error parsing request line: " << line << '\n';
    return false;
}

bool shh::HttpRequest::parse_headers(std::string& line) {
    if (line.empty()) {
        // End of headers, transition to body or finish
        if (should_have_body()) {
            state_ = BODY;
            setup_body_parsing();
        } else {
            state_ = FINISH;
        }
        return true;
    }

    std::regex pattern("^([^:]+):\\s*(.*)$");
    std::smatch subMatch;

    if (std::regex_match(line, subMatch, pattern)) {
        std::string headerName = subMatch[1];
        std::string headerValue = subMatch[2];

        // Trim whitespace from header name
        headerName.erase(0, headerName.find_first_not_of(" \t"));
        headerName.erase(headerName.find_last_not_of(" \t") + 1);

        // Trim whitespace from header value
        headerValue.erase(0, headerValue.find_first_not_of(" \t"));
        headerValue.erase(headerValue.find_last_not_of(" \t") + 1);

        // Convert header name to lowercase
        std::transform(headerName.begin(), headerName.end(), headerName.begin(), ::tolower);

        headers_[headerName] = headerValue;
        return true;
    }

    std::cerr << "Invalid header format: " << line << std::endl;
    return false;
}

bool shh::HttpRequest::should_have_body() const {
    // Check if method typically has a body
    if (method_ == "GET") {
        return false;
    }

    // Check Content-Length header
    auto contentLengthIt = headers_.find("content-length");
    if (contentLengthIt != headers_.end()) {
        try {
            size_t length = std::stoul(contentLengthIt->second);
            return length > 0;
        } catch (const std::exception&) {
            return false;
        }
    }

    // Check Transfer-Encoding header
    auto transferEncodingIt = headers_.find("transfer-encoding");
    if (transferEncodingIt != headers_.end()) {
        return transferEncodingIt->second.find("chunked") != std::string::npos;
    }

    return false;
}

void shh::HttpRequest::setup_body_parsing() {
    auto contentLengthIt = headers_.find("content-length");
    if (contentLengthIt != headers_.end()) {
        try {
            contentLength_ = std::stoul(contentLengthIt->second);
        } catch (const std::exception& e) {
            std::cerr << "Invalid Content-Length: " << contentLengthIt->second << std::endl;
            contentLength_ = 0;
        }
    } else {
        contentLength_ = 0;
    }

    bodyBytesRead_ = 0;
    body_.clear();
}

bool shh::HttpRequest::parse_body_content(shh::Buffer& buffer) {
    if (contentLength_ == 0) {
        state_ = FINISH;
        return true;
    }

    size_t remainingBytes = contentLength_ - bodyBytesRead_;
    size_t availableBytes = buffer.ReadableBytes();
    size_t bytesToRead = std::min(remainingBytes, availableBytes);

    if (bytesToRead > 0) {
        body_.append(buffer.Peek(), bytesToRead);
        buffer.Retrieve(bytesToRead);
        bodyBytesRead_ += bytesToRead;
    }

    if (bodyBytesRead_ >= contentLength_) {
        state_ = FINISH;
        return true;
    }

    // Need more data
    return false;
}

void shh::HttpRequest::parse_path(const std::vector<std::string> files) {
    for (const auto& item : files) {
        if (item == path_) {
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
    contentLength_ = 0;
    bodyBytesRead_ = 0;
}

std::string shh::HttpRequest::getHeader(const std::string& name) const {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    auto it = headers_.find(lowerName);
    return (it != headers_.end()) ? it->second : "";
}
