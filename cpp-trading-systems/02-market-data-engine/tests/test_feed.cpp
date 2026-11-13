#include "feed.hpp"
#include <cassert>
int main() {
    using namespace feed;
    Event e{1,EventType::Add,7,10100,20};
    auto bytes=encode(e); std::string error;
    auto d=decode(bytes,error); assert(d && d->sequence==1 && d->price_ticks==10100);
    bytes.pop_back(); assert(!decode(bytes,error));
    Sequencer s; assert(s.accept(1)==SequenceStatus::InOrder);
    assert(s.accept(3)==SequenceStatus::Gap);
    assert(s.accept(3)==SequenceStatus::DuplicateOrOld);
    TopOfBook b; b.apply(e); assert(b.best_bid() && b.best_bid()->quantity==20);
    b.apply({2,EventType::Cancel,7,10100,20}); assert(!b.best_bid());
}
