#include "feed.hpp"
#include <iostream>
int main() {
    using namespace feed;
    Event event{1,EventType::Add,42,10125,50};
    auto bytes=encode(event); std::string error;
    auto decoded=decode(bytes,error);
    if(!decoded) { std::cerr<<error<<'\n'; return 1; }
    Sequencer seq;
    std::cout<<"Decoded sequence "<<decoded->sequence
             <<", status "<<(seq.accept(decoded->sequence)==SequenceStatus::InOrder?"in-order":"unexpected")<<'\n';
    TopOfBook book; book.apply(*decoded);
    if(auto bid=book.best_bid()) std::cout<<"Best bid: "<<bid->price_ticks<<" x "<<bid->quantity<<'\n';
}
