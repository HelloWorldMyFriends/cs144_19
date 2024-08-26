#include "byte_stream.hh"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <stdexcept>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

using namespace std;

ByteStream::ByteStream(const size_t capacity)
    : _capacity(capacity), _read_count(0), _write_count(0), _input_ended(false){}

size_t ByteStream::write(const string &data) {
    size_t len = min(data.size(), _capacity - _buffer.size());
    for(size_t i = 0; i < len; ++i){
        _buffer.push_back(data[i]);
    }
    _write_count += len;
    return len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {// TODO
    size_t peek_size = min(len, _buffer.size());
    return string(_buffer.begin(), _buffer.begin() + peek_size);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t length = len;
    if(length > _buffer.size()){
        length = _buffer.size();
    }
    _read_count += length;
    while(length--){
        _buffer.pop_front();
    }
    return;
 }

void ByteStream::end_input() { 
    _input_ended = true;
    return;
}

bool ByteStream::input_ended() const { 
    return _input_ended; 
}

size_t ByteStream::buffer_size() const { 
    return _buffer.size(); 
}

bool ByteStream::buffer_empty() const { 
    return (_buffer.size() == 0);
}

bool ByteStream::eof() const { 
    return (buffer_empty() && input_ended());
}

size_t ByteStream::bytes_written() const {
    return _write_count; 
}

size_t ByteStream::bytes_read() const {
    return _read_count; 
}

size_t ByteStream::remaining_capacity() const {
    return _capacity - _buffer.size(); 
}

void ByteStream::CatStream(){
    cout << "current ByteStream is:";
    for(const char &elem : _buffer){
        cout << elem;
      }
      cout << endl;
      return;
}