//
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
#include<vector>

namespace shh {

class Buffer{
private:
    std::vector<char> buffer_;
    int writePos_;
    int readPos_;
public:
    explicit Buffer(int defaultBufSize);

    size_t ReadableBytes() const;
    size_t WritableBytes() const;
    size_t PrependableBytes() const;

    void EnsureWritable(size_t len);
    void HasWritten(size_t len);

    char* BeginWrite();
    const char* BeginWrite() const;
    const char* Peek() const;

    void BufferAppend(const char* str, size_t len);
    void Retrieve(size_t len);
    void RetrieveUntil(const char* end);
    void RetrieveAll();

    const char* FindCRLF() const;
    const char* FindCRLF_CRLF() const; // i apologize for this name...
    ~Buffer() = default;
};

}

#endif
