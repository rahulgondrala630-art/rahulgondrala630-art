#include "order_book.hpp"
#include <algorithm>

namespace matching {
SubmitResult OrderBook::submit(Order incoming) {
    std::lock_guard lock(mutex_);
    SubmitResult result;
    if (incoming.id.empty()) { result.reason = "order id must not be empty"; return result; }
    if (incoming.quantity == 0) { result.reason = "quantity must be greater than zero"; return result; }
    if (active_ids_.contains(incoming.id)) { result.reason = "duplicate active order id"; return result; }
    if (incoming.type == OrderType::Limit && incoming.price_ticks <= 0) {
        result.reason = "limit price must be positive"; return result;
    }

    const auto crosses = [&]() {
        if (incoming.side == Side::Buy) {
            return !asks_.empty() && (incoming.type == OrderType::Market ||
                                      incoming.price_ticks >= asks_.begin()->first);
        }
        return !bids_.empty() && (incoming.type == OrderType::Market ||
                                  incoming.price_ticks <= bids_.begin()->first);
    };

    while (incoming.quantity > 0 && crosses()) {
        if (incoming.side == Side::Buy) {
            auto level = asks_.begin();
            auto& maker = level->second.front();
            const auto traded = std::min(incoming.quantity, maker.quantity);
            result.trades.push_back({maker.id, incoming.id, maker.price_ticks, traded});
            incoming.quantity -= traded;
            maker.quantity -= traded;
            if (maker.quantity == 0) {
                active_ids_.erase(maker.id);
                level->second.pop_front();
                if (level->second.empty()) asks_.erase(level);
            }
        } else {
            auto level = bids_.begin();
            auto& maker = level->second.front();
            const auto traded = std::min(incoming.quantity, maker.quantity);
            result.trades.push_back({maker.id, incoming.id, maker.price_ticks, traded});
            incoming.quantity -= traded;
            maker.quantity -= traded;
            if (maker.quantity == 0) {
                active_ids_.erase(maker.id);
                level->second.pop_front();
                if (level->second.empty()) bids_.erase(level);
            }
        }
    }
    if (incoming.quantity > 0 && incoming.type == OrderType::Limit) rest(std::move(incoming));
    result.accepted = true;
    return result;
}

void OrderBook::rest(Order order) {
    active_ids_.insert(order.id);
    if (order.side == Side::Buy) bids_[order.price_ticks].push_back(std::move(order));
    else asks_[order.price_ticks].push_back(std::move(order));
}

bool OrderBook::cancel(const std::string& id) {
    std::lock_guard lock(mutex_);
    if (!active_ids_.contains(id)) return false;
    for (auto level = bids_.begin(); level != bids_.end(); ++level) {
        auto& q = level->second;
        for (auto it = q.begin(); it != q.end(); ++it) if (it->id == id) {
            q.erase(it); if (q.empty()) bids_.erase(level); active_ids_.erase(id); return true;
        }
    }
    for (auto level = asks_.begin(); level != asks_.end(); ++level) {
        auto& q = level->second;
        for (auto it = q.begin(); it != q.end(); ++it) if (it->id == id) {
            q.erase(it); if (q.empty()) asks_.erase(level); active_ids_.erase(id); return true;
        }
    }
    return false;
}

std::vector<LevelSnapshot> OrderBook::snapshot(const BidLevels& levels, std::size_t depth) {
    std::vector<LevelSnapshot> out;
    for (const auto& [price, q] : levels) {
        if (out.size() == depth) break;
        std::uint64_t total = 0; for (const auto& o : q) total += o.quantity;
        out.push_back({price, total, q.size()});
    }
    return out;
}
std::vector<LevelSnapshot> OrderBook::snapshot(const AskLevels& levels, std::size_t depth) {
    std::vector<LevelSnapshot> out;
    for (const auto& [price, q] : levels) {
        if (out.size() == depth) break;
        std::uint64_t total = 0; for (const auto& o : q) total += o.quantity;
        out.push_back({price, total, q.size()});
    }
    return out;
}
std::vector<LevelSnapshot> OrderBook::bids(std::size_t depth) const {
    std::lock_guard lock(mutex_); return snapshot(bids_, depth);
}
std::vector<LevelSnapshot> OrderBook::asks(std::size_t depth) const {
    std::lock_guard lock(mutex_); return snapshot(asks_, depth);
}
std::size_t OrderBook::order_count() const {
    std::lock_guard lock(mutex_); return active_ids_.size();
}
} // namespace matching
