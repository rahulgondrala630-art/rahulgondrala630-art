#include "order_book.hpp"
#include <cassert>
int main() {
    using namespace matching;
    OrderBook book;
    assert(book.submit({"b1",Side::Buy,OrderType::Limit,100,10}).accepted);
    assert(book.submit({"b2",Side::Buy,OrderType::Limit,100,5}).accepted);
    auto s = book.submit({"s1",Side::Sell,OrderType::Limit,100,12});
    assert(s.accepted && s.trades.size()==2);
    assert(s.trades[0].maker_order_id=="b1" && s.trades[0].quantity==10);
    assert(s.trades[1].maker_order_id=="b2" && s.trades[1].quantity==2);
    assert(book.order_count()==1);
    assert(book.cancel("b2"));
    assert(!book.cancel("b2"));
    assert(book.order_count()==0);
    auto market = book.submit({"m1",Side::Sell,OrderType::Market,0,1});
    assert(market.accepted && market.trades.empty());
    assert(book.order_count()==0);
    auto invalid = book.submit({"bad",Side::Buy,OrderType::Limit,0,1});
    assert(!invalid.accepted);
}
