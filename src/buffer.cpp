//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "buffer.hpp"
#include <assert.h>
shh::Buffer::Buffer(int defaultBufSize) : buffer_(defaultBufSize), readPos_(0), writePos_(0) {}

size_t shh::Buffer::ReadableBytes() const{
    return writePos_ - readPos_;
}

size_t shh::Buffer::WritableBytes() const{
    return buffer_.size() - writePos_;
}

size_t shh::Buffer::PrependableBytes() const{
    return readPos_;
}

void shh::Buffer::EnsureWritable(size_t len){
    if (WritableBytes() < len){
    }
    assert(WritableBytes() >= len);
}
void shh::Buffer::BufferAppend(const char* str, size_t len) {
    assert(str);
    EnsureWritable(len);
    std::copy(str, str + len, BeginWrite())
    HasWritten(len);
}
