#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
namespace feed {
enum class EventType : std::uint8_t { Add=1, Modify=2, Cancel=3, Trade=4 };
struct Event {
    std::uint64_t sequence{};
    EventType type{};
    std::uint64_t order_id{};
    std::int64_t price_ticks{};
    std::uint64_t quantity{};
    bool is_buy{true};
};
std::vector<std::uint8_t> encode(const Event& event);
std::optional<Event> decode(const std::vector<std::uint8_t>& bytes, std::string& error);
enum class SequenceStatus { InOrder, Gap, DuplicateOrOld };
class Sequencer {
public:
    SequenceStatus accept(std::uint64_t sequence);
    std::uint64_t expected() const { return expected_; }
private:
    std::uint64_t expected_{1};
    bool exhausted_{false};
};
struct BookLevel { std::int64_t price_ticks{}; std::uint64_t quantity{}; };
class TopOfBook {
public:
    void apply(const Event& event);
    std::optional<BookLevel> best_bid() const;
    std::optional<BookLevel> best_ask() const;
private:
    struct Entry { bool buy; std::int64_t price; std::uint64_t quantity; };
    std::unordered_map<std::uint64_t,Entry> orders_;
};
} // namespace feed
