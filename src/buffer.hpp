//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#pragma once
#ifndef buffer_h
#define buffer_h

#include<cstddef>
#include<string>
#include<vector>

namespace shh {

class Buffer{
private:
    std::vector<char> buffer_;
    int writePos_;
    int readPos_;
public:
    Buffer(int defaultBufSize);
    size_t ReadableBytes() const;
    size_t WritableBytes() const;
    size_t PrependableBytes() const;
    void EnsureWritable(size_t len);
    void HasWritten(size_t len);
    char* BeginWrite();
    void BufferAppend(const char* str, size_t len);

    ~Buffer() = default;
};

}

#endif
