#include "execution.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
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
    if(!std::isfinite(sum)||sum<=0) return {};
    std::uint64_t allocated=0;
    for(std::size_t i=0;i<curve.size();++i) {
        const auto q=(i+1==curve.size())?total-allocated:static_cast<std::uint64_t>(std::floor(static_cast<long double>(total)*curve[i]/sum));
        allocated+=q; out.push_back({q,(i*duration)/curve.size()});
    }
    return out;
}
RiskDecision check_order(std::uint64_t q,std::int64_t p,double tick,const RiskLimits& l) {
    if(!q||p<=0||!std::isfinite(tick)||tick<=0) return {false,"quantity, price and tick value must be positive"};
    if(q>l.max_order_quantity) return {false,"order quantity limit exceeded"};
    const long double notional=static_cast<long double>(q)*static_cast<long double>(p)*tick;
    if(!std::isfinite(notional)||notional>l.max_notional) return {false,"notional limit exceeded"};
    return {true,"approved"};
}
const Venue* best_venue(const std::vector<Venue>& venues) {
    const Venue* best=nullptr; long double best_cost=0;
    for(const auto& v:venues) if(v.available&&v.available_quantity>0&&v.ask_ticks>0&&std::isfinite(v.fee_per_unit)&&v.fee_per_unit>=0) {
        const long double cost=static_cast<long double>(v.ask_ticks)+v.fee_per_unit;
        if(!best||cost<best_cost) { best=&v; best_cost=cost; }
    }
    return best;
}
Metrics analyze(const std::vector<Fill>& fills,std::uint64_t requested,std::int64_t arrival) {
    Metrics m; long double weighted=0;
    for(const auto& f:fills) {
        if(!f.quantity||f.price_ticks<=0||!std::isfinite(f.fees)||f.fees<0) continue;
        if(f.quantity>std::numeric_limits<std::uint64_t>::max()-m.filled) continue;
        m.filled+=f.quantity; weighted+=static_cast<long double>(f.quantity)*f.price_ticks; m.fees+=f.fees;
    }
    if(m.filled) m.average_price_ticks=static_cast<double>(weighted/m.filled);
    if(requested) m.fill_ratio=static_cast<double>(m.filled)/static_cast<double>(requested);
    if(arrival>0&&m.filled) m.slippage_ticks=m.average_price_ticks-static_cast<double>(arrival);
    return m;
}
ParentOrder::ParentOrder(std::uint64_t target):target_(target) {
    if(target_==0) status_=ParentStatus::Cancelled;
}
bool ParentOrder::record_fill(const Fill& fill) {
    if(status_==ParentStatus::Cancelled||status_==ParentStatus::Filled||!fill.quantity||fill.price_ticks<=0||
       !std::isfinite(fill.fees)||fill.fees<0||fill.quantity>remaining_quantity()) return false;
    filled_+=fill.quantity;
    status_=(filled_==target_)?ParentStatus::Filled:ParentStatus::PartiallyFilled;
    return true;
}
void ParentOrder::cancel() {
    if(status_==ParentStatus::Open||status_==ParentStatus::PartiallyFilled) status_=ParentStatus::Cancelled;
}
std::vector<Fill> execute_buy_child(const Venue& venue,std::uint64_t requested) {
    if(!requested||!venue.available||venue.ask_ticks<=0||!std::isfinite(venue.fee_per_unit)||venue.fee_per_unit<0) return {};
    const auto quantity=std::min(requested,venue.available_quantity);
    if(!quantity) return {};
    return {{quantity,venue.ask_ticks,static_cast<double>(quantity)*venue.fee_per_unit}};
}
} // namespace execution
