#include "order_book.hpp"
#include <cassert>
int main() {
    using namespace matching;
    OrderBook book;
    assert(book.submit({"b1",Side::Buy,OrderType::Limit,100,10}).accepted);
    assert(book.submit({"b2",Side::Buy,OrderType::Limit,100,5}).accepted);
    auto s=book.submit({"s1",Side::Sell,OrderType::Limit,100,12});
    assert(s.accepted && s.trades.size()==2 && s.remaining_quantity==0);
    assert(s.trades[0].maker_order_id=="b1" && s.trades[0].quantity==10);
    assert(s.trades[1].maker_order_id=="b2" && s.trades[1].quantity==2);
    assert(book.order_count()==1 && book.cancel("b2") && !book.cancel("b2"));
    assert(book.order_count()==0);
    assert(book.submit({"ask",Side::Sell,OrderType::Limit,105,10}).accepted);
    auto fok_fail=book.submit({"fok-fail",Side::Buy,OrderType::Limit,105,11,TimeInForce::FillOrKill});
    assert(!fok_fail.accepted && book.order_count()==1);
    auto fok=book.submit({"fok",Side::Buy,OrderType::Limit,105,10,TimeInForce::FillOrKill});
    assert(fok.accepted && fok.trades.size()==1 && book.order_count()==0);
    assert(book.submit({"ask2",Side::Sell,OrderType::Limit,106,4}).accepted);
    auto ioc=book.submit({"ioc",Side::Buy,OrderType::Limit,106,10,TimeInForce::ImmediateOrCancel});
    assert(ioc.accepted && ioc.trades.size()==1 && ioc.remaining_quantity==6);
    assert(book.order_count()==0);
    auto market=book.submit({"market",Side::Buy,OrderType::Market,0,2});
    assert(market.accepted && market.remaining_quantity==2);
    assert(!book.submit({"bad",Side::Buy,OrderType::Limit,0,1}).accepted);
}
