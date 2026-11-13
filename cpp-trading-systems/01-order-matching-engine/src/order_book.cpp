#include "order_book.hpp"
#include <algorithm>
#include <limits>
#include <iterator>
#include <utility>
namespace matching {
bool OrderBook::crosses(const Order& o) const {
    if(o.side==Side::Buy) return !asks_.empty() &&
        (o.type==OrderType::Market || o.price_ticks>=asks_.begin()->first);
    return !bids_.empty() && (o.type==OrderType::Market || o.price_ticks<=bids_.begin()->first);
}
std::uint64_t OrderBook::executable_quantity(const Order& o) const {
    std::uint64_t total=0;
    if(o.side==Side::Buy) {
        for(const auto& [price,q]:asks_) {
            if(o.type==OrderType::Limit && price>o.price_ticks) break;
            for(const auto& maker:q) {
                if(maker.quantity>std::numeric_limits<std::uint64_t>::max()-total) return std::numeric_limits<std::uint64_t>::max();
                total+=maker.quantity;
            }
        }
    } else {
        for(const auto& [price,q]:bids_) {
            if(o.type==OrderType::Limit && price<o.price_ticks) break;
            for(const auto& maker:q) {
                if(maker.quantity>std::numeric_limits<std::uint64_t>::max()-total) return std::numeric_limits<std::uint64_t>::max();
                total+=maker.quantity;
            }
        }
    }
    return total;
}
SubmitResult OrderBook::submit(Order incoming) {
    std::lock_guard lock(mutex_);
    SubmitResult result;
    if(incoming.id.empty()) { result.reason="order id must not be empty"; return result; }
    if(incoming.quantity==0) { result.reason="quantity must be greater than zero"; return result; }
    if(index_.contains(incoming.id)) { result.reason="duplicate active order id"; return result; }
    if(incoming.type==OrderType::Limit && incoming.price_ticks<=0) { result.reason="limit price must be positive"; return result; }
    if(incoming.type==OrderType::Market && incoming.time_in_force==TimeInForce::GoodTillCancelled)
        incoming.time_in_force=TimeInForce::ImmediateOrCancel;
    if(incoming.time_in_force==TimeInForce::FillOrKill &&
       executable_quantity(incoming)<incoming.quantity) {
        result.reason="fill-or-kill order cannot be filled completely";
        return result;
    }
    while(incoming.quantity>0 && crosses(incoming)) {
        if(incoming.side==Side::Buy) {
            auto level=asks_.begin(); auto& maker=level->second.front();
            const auto qty=std::min(incoming.quantity,maker.quantity);
            result.trades.push_back({maker.id,incoming.id,maker.price_ticks,qty});
            incoming.quantity-=qty; maker.quantity-=qty;
            if(maker.quantity==0) erase_front(Side::Sell,level->first);
        } else {
            auto level=bids_.begin(); auto& maker=level->second.front();
            const auto qty=std::min(incoming.quantity,maker.quantity);
            result.trades.push_back({maker.id,incoming.id,maker.price_ticks,qty});
            incoming.quantity-=qty; maker.quantity-=qty;
            if(maker.quantity==0) erase_front(Side::Buy,level->first);
        }
    }
    result.remaining_quantity=incoming.quantity;
    if(incoming.quantity>0 && incoming.type==OrderType::Limit &&
       incoming.time_in_force==TimeInForce::GoodTillCancelled) rest(std::move(incoming));
    result.accepted=true;
    return result;
}
void OrderBook::rest(Order order) {
    if(order.side==Side::Buy) {
        auto& q=bids_[order.price_ticks]; q.push_back(std::move(order));
        auto it=std::prev(q.end()); index_.emplace(it->id,Location{Side::Buy,it->price_ticks,it});
    } else {
        auto& q=asks_[order.price_ticks]; q.push_back(std::move(order));
        auto it=std::prev(q.end()); index_.emplace(it->id,Location{Side::Sell,it->price_ticks,it});
    }
}
void OrderBook::erase_front(Side side,std::int64_t price) {
    if(side==Side::Buy) {
        auto level=bids_.find(price); auto it=level->second.begin();
        index_.erase(it->id); level->second.erase(it); if(level->second.empty()) bids_.erase(level);
    } else {
        auto level=asks_.find(price); auto it=level->second.begin();
        index_.erase(it->id); level->second.erase(it); if(level->second.empty()) asks_.erase(level);
    }
}
bool OrderBook::cancel(const std::string& id) {
    std::lock_guard lock(mutex_);
    auto found=index_.find(id); if(found==index_.end()) return false;
    const auto location=found->second;
    if(location.side==Side::Buy) {
        auto level=bids_.find(location.price); level->second.erase(location.iterator);
        if(level->second.empty()) bids_.erase(level);
    } else {
        auto level=asks_.find(location.price); level->second.erase(location.iterator);
        if(level->second.empty()) asks_.erase(level);
    }
    index_.erase(found); return true;
}
std::vector<LevelSnapshot> OrderBook::snapshot(const BidLevels& levels,std::size_t depth) {
    std::vector<LevelSnapshot> out;
    for(const auto& [price,q]:levels) { if(out.size()==depth) break; std::uint64_t total=0;
        for(const auto& o:q) {
            if(o.quantity>std::numeric_limits<std::uint64_t>::max()-total) {
                total=std::numeric_limits<std::uint64_t>::max(); break;
            }
            total+=o.quantity;
        }
        out.push_back({price,total,q.size()}); }
    return out;
}
std::vector<LevelSnapshot> OrderBook::snapshot(const AskLevels& levels,std::size_t depth) {
    std::vector<LevelSnapshot> out;
    for(const auto& [price,q]:levels) { if(out.size()==depth) break; std::uint64_t total=0;
        for(const auto& o:q) total+=o.quantity; out.push_back({price,total,q.size()}); }
    return out;
}
std::vector<LevelSnapshot> OrderBook::bids(std::size_t depth) const { std::lock_guard lock(mutex_); return snapshot(bids_,depth); }
std::vector<LevelSnapshot> OrderBook::asks(std::size_t depth) const { std::lock_guard lock(mutex_); return snapshot(asks_,depth); }
std::size_t OrderBook::order_count() const { std::lock_guard lock(mutex_); return index_.size(); }
} // namespace matching
