#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
namespace matching {
enum class Side { Buy, Sell };
enum class OrderType { Limit, Market };
enum class TimeInForce { GoodTillCancelled, ImmediateOrCancel, FillOrKill };
struct Order {
    std::string id;
    Side side{};
    OrderType type{OrderType::Limit};
    std::int64_t price_ticks{};
    std::uint64_t quantity{};
    TimeInForce time_in_force{TimeInForce::GoodTillCancelled};
};
struct Trade { std::string maker_order_id; std::string taker_order_id; std::int64_t price_ticks{}; std::uint64_t quantity{}; };
struct SubmitResult { bool accepted{false}; std::string reason; std::vector<Trade> trades; std::uint64_t remaining_quantity{}; };
struct LevelSnapshot { std::int64_t price_ticks{}; std::uint64_t quantity{}; std::size_t order_count{}; };
class OrderBook {
public:
    SubmitResult submit(Order order);
    bool cancel(const std::string& order_id);
    std::vector<LevelSnapshot> bids(std::size_t depth = 10) const;
    std::vector<LevelSnapshot> asks(std::size_t depth = 10) const;
    std::size_t order_count() const;
private:
    using Queue = std::list<Order>;
    using Iterator = Queue::iterator;
    using BidLevels = std::map<std::int64_t, Queue, std::greater<>>;
    using AskLevels = std::map<std::int64_t, Queue>;
    struct Location { Side side; std::int64_t price; Iterator iterator; };
    mutable std::mutex mutex_;
    BidLevels bids_;
    AskLevels asks_;
    std::unordered_map<std::string, Location> index_;
    bool crosses(const Order&) const;
    std::uint64_t executable_quantity(const Order&) const;
    void rest(Order order);
    void erase_front(Side side, std::int64_t price);
    static std::vector<LevelSnapshot> snapshot(const BidLevels&, std::size_t);
    static std::vector<LevelSnapshot> snapshot(const AskLevels&, std::size_t);
};
} // namespace matching
