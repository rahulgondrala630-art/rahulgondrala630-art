#include "execution.hpp"
#include <cassert>
int main() {
    using namespace execution;
    auto t=twap(101,4,60); assert(t.size()==4);
    std::uint64_t sum=0; for(auto x:t) sum+=x.quantity; assert(sum==101);
    auto v=vwap(100,{1.0,2.0,1.0},60); sum=0; for(auto x:v) sum+=x.quantity; assert(sum==100);
    assert(!vwap(10,{0.0,0.0},10).size());
    auto ok=check_order(10,100,0.01,{20,100}); assert(ok.allowed);
    auto no=check_order(30,100,0.01,{20,100}); assert(!no.allowed);
    auto venue=best_venue({{"a",101,0.1,true},{"b",100,0.5,true}});
    assert(venue && venue->name=="b");
    auto m=analyze({{5,102,1.0}},10,100);
    assert(m.filled==5 && m.fill_ratio==0.5 && m.slippage_ticks==2.0);
}
