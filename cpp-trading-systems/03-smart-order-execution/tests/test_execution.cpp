#include "execution.hpp"
#include <cassert>
int main() {
    using namespace execution;
    auto t=twap(101,4,60); assert(t.size()==4);
    std::uint64_t sum=0; for(auto x:t) sum+=x.quantity; assert(sum==101);
    auto v=vwap(100,{1.0,2.0,1.0},60); sum=0; for(auto x:v) sum+=x.quantity; assert(sum==100);
    assert(vwap(10,{0.0,0.0},10).empty());
    assert(check_order(10,100,0.01,{20,100}).allowed);
    assert(!check_order(30,100,0.01,{20,100}).allowed);
    std::vector<Venue> venues{{"a",101,0.1,10,true},{"b",100,0.5,20,true}};
    auto venue=best_venue(venues); assert(venue&&venue->name=="b");
    auto fills=execute_buy_child(*venue,30); assert(fills.size()==1&&fills[0].quantity==20);
    auto m=analyze(fills,30,99); assert(m.filled==20&&m.fill_ratio>0.66&&m.fill_ratio<0.67);
    ParentOrder parent(30); assert(parent.record_fill(fills[0]));
    assert(parent.status()==ParentStatus::PartiallyFilled&&parent.remaining_quantity()==10);
    parent.cancel(); assert(parent.status()==ParentStatus::Cancelled);
    assert(!parent.record_fill({1,100,0}));
    ParentOrder complete(5); assert(complete.record_fill({5,100,0}));
    assert(complete.status()==ParentStatus::Filled&&!complete.record_fill({1,100,0}));
}
