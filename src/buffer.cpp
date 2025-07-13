//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include "buffer.hpp"
#include <assert.h>
#include <cstring>
#include <algorithm>

shh::Buffer::Buffer(int defaultBufSize) : buffer_(defaultBufSize), readPos_(0), writePos_(0) {
    assert(defaultBufSize > 0);
}

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
        if (PrependableBytes() + WritableBytes() >= len){
            //move readable data to front
            size_t readable = ReadableBytes();
            std::memmove(buffer_.data(), buffer_.data() + readPos_, readable);
            readPos_ = 0;
            writePos_ = readable;
        } else {
            size_t new_size = std::max(buffer_.size() * 2, writePos_ + len);
            buffer_.resize(new_size);
        }
    }
    assert(WritableBytes() >= len);
}

void shh::Buffer::HasWritten(size_t len){
    writePos_ += len;
}

void shh::Buffer::BufferAppend(const char* str, size_t len) {
    assert(str);
    EnsureWritable(len);
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

char* shh::Buffer::BeginWrite(){
    return buffer_.data() + writePos_;
}

const char* shh::Buffer::BeginWrite() const{
    //for const methods to call...
    return buffer_.data() + writePos_;
}
const char* shh::Buffer::Peek() const {
    return buffer_.data() + readPos_;
}

void shh::Buffer::Retrieve(size_t len){
    assert(len <= ReadableBytes());
    readPos_ += len;
}

void shh::Buffer::RetrieveUntil(const char* end){

    assert(end >= Peek()); //ensure end is within readable region
    assert(end <= BeginWrite()); //ensure end doesn't exceede writable bytes

    size_t len = end-Peek();
    Retrieve(len);
}

void shh::Buffer::RetrieveAll(){
    // reset buffer without clearing memory.
    readPos_ = 0;
    writePos_ = 0;
}

const char* shh::Buffer::FindCRLF() const {
    // Search for HTTP line terminator "\r\n" in readable data
    // Returns pointer to '\r' if found, nullptr otherwise
    const char* crlf = "\r\n";
    const char* found = std::search(Peek(), BeginWrite(), crlf, crlf + 2);

    return found == BeginWrite() ? nullptr : found;
}

const char* shh::Buffer::FindCRLF_CRLF() const {
    // Search for HTTP header/body separator "\r\n\r\n"
    // Returns pointer to '\r' if found, nullptr otherwise
    const char* double_crlf = "\r\n\r\n";
    const char* found = std::search(Peek(), BeginWrite(), double_crlf, double_crlf + 4);

    return found == BeginWrite() ? nullptr : found;
}

void shh::Buffer::BufferAppend(const std::string& str) {
    BufferAppend(str.data(), str.length());
}
