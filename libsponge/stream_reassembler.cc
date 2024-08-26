#include "stream_reassembler.hh"
#include <iostream>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) 
: _output(capacity), _capacity(capacity), _buffer(), _assembled_bytes(0), _input_ended(false), _eof_index(-1) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if(index >= _head_index + _capacity){
        return;
    }

    if(eof){
        _eof_index = index + data.size();
    }

    // Insert the data into the buffer, and make sure that not over capacity
    for(size_t i = 0; i < data.size(); ++i){
        size_t pos = index + i;
        if(pos < _assembled_bytes || _buffer.count(pos) || pos >= _head_index + _capacity){
            continue;
        }
        _buffer[pos] = data[i];
    }

    // // Calculate bytes of already pushed
    // size_t bytes_to_write = 0;
    // for(size_t pos = _assembled_bytes; pos < _head_index + _capacity && _buffer.count(pos); ++pos){
    //     bytes_to_write++;
    // }

    // 计算当前可以写入 ByteStream 的字节数
    size_t bytes_to_write = 0;
    while (_buffer.count(_assembled_bytes + bytes_to_write) && 
           bytes_to_write < (_capacity - _output.buffer_size())) {
        bytes_to_write++;
    }

    cout<<"push_substring(): capacity is "<<_capacity<<endl;
    // Write contiguous data to the output stream
   string contiguous_data;
    for (size_t i = 0; i < bytes_to_write; ++i) {
        contiguous_data += _buffer[_assembled_bytes + i];
    }

    _output.write(contiguous_data);
    _assembled_bytes += bytes_to_write;
    _head_index += bytes_to_write;

    // 清除已经写入的数据
    for (size_t i = 0; i < bytes_to_write; ++i) {
        _buffer.erase(_assembled_bytes - bytes_to_write + i);
    }

    // 当所有子串都被组装完毕时，结束输入
    if (_assembled_bytes == _eof_index) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { 
    return _buffer.size(); 
}

bool StreamReassembler::empty() const { 
    return _buffer.empty(); 
}
