#include <iostream>
#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.


using namespace std;

bool TCPReceiver::segment_received(const TCPSegment &seg) {
    
    //Extract sequence number and data from the segment
    bool ret = false;
    bool syn = seg.header().syn;
    bool fin = seg.header().fin;
    size_t abs_seqno = 0;
    size_t length;
    static int i = 0;
    //sequence length except syn
    std::string data = seg.payload().copy();
    
    // if(_fin_flag){
    //     return false;
    // }

    if(syn){    // if receive SYN segment
        if(_syn_flag){  // already received SYN
            return false;
        }

        ret = true;
        _isn = seg.header().seqno;
        _syn_flag = true;
        _checkpoint = 1;
        abs_seqno = 1;
        _base = 1;
        length = seg.length_in_sequence_space() - 1;
        // cout<<"synsegment length: "<<length<<endl;
        cout<<"-----------test "<<i++<< " is here------------"<<endl;
        cout<<"isn: "<<_isn.value()<<endl;
        if(length == 0)
            return true; //one byte SYN segment

    } else if(!_syn_flag){  // haven't receive SYN, refuse other segment
        
        return ret;

    } else {    // not a SYN segment, compute its abs_seqno and data length
        // abs_seqno = unwrap(seg.header().seqno, _isn.value(), _checkpoint);
        abs_seqno = unwrap(seg.header().seqno, _isn.value(), _checkpoint);

        length = seg.length_in_sequence_space();
        // cout<<"segment length awa: "<<length<<endl;
        // cout<<"abs_seqno awa: "<<abs_seqno<<endl;
    }

    
    if(fin){
        if(_fin_flag){  //already have received FIN segment, refuse other FIN segment
            return false;
        }

        _fin_flag = true;
        ret = true;
    
    }
    //not FIN segment or SYN flag only, should check border

    else if((seg.length_in_sequence_space() == 0) && (abs_seqno == _base)){
        return true;
    } 
    else if(abs_seqno >= _base + window_size() || abs_seqno + length <= _base){
        if(!ret)
            return false;
    }
    cout<<"start--------"<<endl;
    cout<<endl<<"abs seqno is: "<<abs_seqno<<endl;
    // cout<<endl<<"checkpoint is: "<<_checkpoint<<endl;
    // cout<<endl<<"base is: "<<_base<<endl;
    // cout<<endl<<"segmentlength is: "<<length<<endl;
    
    cout<<endl<<"datasize is: "<<data.size()<<" seqno is: "<<seg.header().seqno<<" data is:"<<data<<endl;
    cout<<"start to push_substring"<<endl;

    _reassembler.push_substring(data, abs_seqno - 1, seg.header().fin);
    _checkpoint = max(_checkpoint, abs_seqno + data.size());
    _base = _reassembler.head_index() + 1;    // TODO

    cout<<endl<<"window.size() = "<<window_size()<<endl;
    cout<<endl<<"_reassembler.head_index() = "<<_reassembler.head_index()<<endl;
    cout<<endl<<"updated checkpoint is: "<<_checkpoint<<endl;
    cout<<endl<<"updayed base is: "<<_base<<endl;
    cout<<"ackno is:"<<wrap(_base, _isn.value())<<endl;
    _reassembler.stream_out().CatStream();
    cout<<"end--------"<<endl<<endl;


    if(_reassembler.stream_out().input_ended()){
        _base++;
    }

    return true;
}

optional<WrappingInt32> TCPReceiver::ackno() const { 
    if(_base > 0){
        
        // cout<<"ackno() _isn is:"<<_isn.value()<<endl;
        
        return wrap(_base, _isn.value());

    }
    else
        return nullopt;
}

size_t TCPReceiver::window_size() const { 
    return _capacity - _reassembler.stream_out().buffer_size(); 
}
