#include "execution.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
namespace execution {
std::vector<ChildOrder> twap(std::uint64_t total,std::uint64_t slices,std::uint64_t duration) {
    std::vector<ChildOrder> out; if(!total||!slices||!duration) return out;
    const auto count=std::min(slices,duration);
    const auto base=total/count, extra=total%count;
    for(std::uint64_t i=0;i<count;++i) out.push_back({base+(i<extra?1:0),(i*duration)/count});
    return out;
}
std::vector<ChildOrder> vwap(std::uint64_t total,const std::vector<double>& curve,std::uint64_t duration) {
    std::vector<ChildOrder> out; if(!total||curve.empty()||!duration) return out;
    double sum=0; for(double x:curve) { if(!std::isfinite(x)||x<0) return {}; sum+=x; }
    if(sum<=0) return {};
    std::uint64_t allocated=0;
    for(std::size_t i=0;i<curve.size();++i) {
        auto q=(i+1==curve.size())?total-allocated:static_cast<std::uint64_t>(std::floor(total*curve[i]/sum));
        allocated+=q; out.push_back({q,(i*duration)/curve.size()});
    }
    return out;
}
RiskDecision check_order(std::uint64_t q,std::int64_t p,double tick,const RiskLimits& l) {
    if(!q||p<=0||!std::isfinite(tick)||tick<=0) return {false,"quantity, price and tick value must be positive"};
    if(q>l.max_order_quantity) return {false,"order quantity limit exceeded"};
    const double notional=static_cast<double>(q)*static_cast<double>(p)*tick;
    if(!std::isfinite(notional)||notional>l.max_notional) return {false,"notional limit exceeded"};
    return {true,"approved"};
}
const Venue* best_venue(const std::vector<Venue>& venues) {
    const Venue* best=nullptr; double best_cost=0;
    for(const auto& v:venues) if(v.available&&v.ask_ticks>0&&std::isfinite(v.fee_per_unit)&&v.fee_per_unit>=0) {
        const double cost=static_cast<double>(v.ask_ticks)+v.fee_per_unit;
        if(!best||cost<best_cost) { best=&v; best_cost=cost; }
    }
    return best;
}
Metrics analyze(const std::vector<Fill>& fills,std::uint64_t requested,std::int64_t arrival) {
    Metrics m; double weighted=0;
    for(const auto& f:fills) { if(!f.quantity||f.price_ticks<=0) continue; m.filled+=f.quantity; weighted+=static_cast<double>(f.quantity)*f.price_ticks; m.fees+=f.fees; }
    if(m.filled) m.average_price_ticks=weighted/static_cast<double>(m.filled);
    if(requested) m.fill_ratio=static_cast<double>(m.filled)/static_cast<double>(requested);
    if(arrival>0&&m.filled) m.slippage_ticks=m.average_price_ticks-static_cast<double>(arrival);
    return m;
}
} // namespace execution
