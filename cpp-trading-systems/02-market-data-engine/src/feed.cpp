#include "feed.hpp"
#include <algorithm>
#include <limits>
#include <unordered_map>
namespace feed {
namespace {
void put(std::vector<std::uint8_t>& b, std::uint64_t v, int n) {
    for (int i=n-1;i>=0;--i) b.push_back(static_cast<std::uint8_t>((v>>(i*8))&0xff));
}
bool get(const std::vector<std::uint8_t>& b, std::size_t& p, int n, std::uint64_t& v) {
    if (p+static_cast<std::size_t>(n)>b.size()) return false;
    v=0; for(int i=0;i<n;++i) v=(v<<8)|b[p++]; return true;
}
}
std::vector<std::uint8_t> encode(const Event& e) {
    std::vector<std::uint8_t> b; b.reserve(33);
    put(b,e.sequence,8); put(b,static_cast<std::uint8_t>(e.type),1);
    put(b,e.order_id,8); put(b,static_cast<std::uint64_t>(e.price_ticks),8); put(b,e.quantity,8);
    return b;
}
std::optional<Event> decode(const std::vector<std::uint8_t>& b, std::string& error) {
    if (b.size()!=33) { error="message must be exactly 33 bytes"; return std::nullopt; }
    std::size_t p=0; std::uint64_t seq=0,type=0,id=0,price=0,qty=0;
    if(!get(b,p,8,seq)||!get(b,p,1,type)||!get(b,p,8,id)||!get(b,p,8,price)||!get(b,p,8,qty)) {
        error="truncated message"; return std::nullopt;
    }
    if(type<1||type>4) { error="unknown event type"; return std::nullopt; }
    Event e{seq,static_cast<EventType>(type),id,static_cast<std::int64_t>(price),qty};
    if(e.order_id==0 || e.price_ticks<=0 || e.quantity==0) { error="invalid event fields"; return std::nullopt; }
    return e;
}
SequenceStatus Sequencer::accept(std::uint64_t s) {
    if(s<expected_) return SequenceStatus::DuplicateOrOld;
    if(s>expected_) { expected_=s+1; return SequenceStatus::Gap; }
    ++expected_; return SequenceStatus::InOrder;
}
void TopOfBook::apply(const Event& e) {
    if(e.type==EventType::Add) orders_[e.order_id]={true,e.price_ticks,e.quantity};
    else if(e.type==EventType::Modify) {
        auto it=orders_.find(e.order_id); if(it!=orders_.end()) { it->second.price=e.price_ticks; it->second.quantity=e.quantity; }
    } else if(e.type==EventType::Cancel) orders_.erase(e.order_id);
    else if(e.type==EventType::Trade) {
        auto it=orders_.find(e.order_id); if(it!=orders_.end()) {
            if(e.quantity>=it->second.quantity) orders_.erase(it); else it->second.quantity-=e.quantity;
        }
    }
}
std::optional<BookLevel> TopOfBook::best_bid() const {
    std::optional<BookLevel> best;
    for(const auto& [id,o]:orders_) { (void)id; if(o.buy&&(!best||o.price>best->price_ticks)) best=BookLevel{o.price,o.quantity}; }
    return best;
}
std::optional<BookLevel> TopOfBook::best_ask() const {
    std::optional<BookLevel> best;
    for(const auto& [id,o]:orders_) { (void)id; if(!o.buy&&(!best||o.price<best->price_ticks)) best=BookLevel{o.price,o.quantity}; }
    return best;
}
} // namespace feed
