#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace execution {
struct ChildOrder { std::uint64_t quantity{}; std::uint64_t due_second{}; };
std::vector<ChildOrder> twap(std::uint64_t total, std::uint64_t slices, std::uint64_t duration_seconds);
std::vector<ChildOrder> vwap(std::uint64_t total, const std::vector<double>& volume_curve, std::uint64_t duration_seconds);
struct RiskLimits { std::uint64_t max_order_quantity{}; double max_notional{}; };
struct RiskDecision { bool allowed{}; std::string reason; };
RiskDecision check_order(std::uint64_t quantity, std::int64_t price_ticks, double tick_value, const RiskLimits& limits);
struct Venue { std::string name; std::int64_t ask_ticks{}; double fee_per_unit{}; bool available{true}; };
const Venue* best_venue(const std::vector<Venue>& venues);
struct Fill { std::uint64_t quantity{}; std::int64_t price_ticks{}; double fees{}; };
struct Metrics { std::uint64_t filled{}; double average_price_ticks{}; double fees{}; double fill_ratio{}; double slippage_ticks{}; };
Metrics analyze(const std::vector<Fill>& fills, std::uint64_t requested, std::int64_t arrival_price_ticks);
} // namespace execution
