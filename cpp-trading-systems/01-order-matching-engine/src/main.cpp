#include "order_book.hpp"
#include <iostream>
int main() {
    matching::OrderBook book;
    using namespace matching;
    book.submit({"B-001", Side::Buy, OrderType::Limit, 10000, 10});
    book.submit({"B-002", Side::Buy, OrderType::Limit, 10000, 5});
    auto result = book.submit({"S-001", Side::Sell, OrderType::Limit, 9990, 12});
    if (!result.accepted) { std::cerr << result.reason << '\n'; return 1; }
    for (const auto& t : result.trades)
        std::cout << "Trade " << t.maker_order_id << " -> " << t.taker_order_id
                  << " price_ticks=" << t.price_ticks << " qty=" << t.quantity << '\n';
    std::cout << "Remaining active orders: " << book.order_count() << '\n';
}
