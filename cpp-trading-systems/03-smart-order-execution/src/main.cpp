#include "execution.hpp"
#include <iostream>
int main() {
    using namespace execution;
    auto schedule=twap(1000,5,300);
    RiskLimits limits{500,1000000.0};
    auto risk=check_order(200,10000,0.01,limits);
    std::vector<Venue> venues{{"Venue-A",10002,0.2,500,true},{"Venue-B",10001,0.5,250,true}};
    auto venue=best_venue(venues);
    ParentOrder parent(200);
    if(risk.allowed&&venue) {
        auto fills=execute_buy_child(*venue,200);
        for(const auto& fill:fills) parent.record_fill(fill);
    }
    std::cout<<"TWAP child orders: "<<schedule.size()
             <<"\nRisk: "<<risk.reason
             <<"\nSelected venue: "<<(venue?venue->name:"none")
             <<"\nParent filled: "<<parent.filled_quantity()<<"/"<<parent.target_quantity()<<'\n';
}
