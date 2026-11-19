// #include <concepts>
#include "order_book.hpp"
#include <iostream>
#include <vector>

void print_trades(const std::string& label, const std::vector<Trade>& trades) {
    std::cout << "[" << label << "] Executions: " << trades.size() << "\n";
    for (const auto& t : trades) {
        std::cout << "  TRADE | BuyID: " << t.buy_order_id
                  << " | SellID: " << t.sell_order_id
                  << " | Price: " << t.price
                  << " | Qty: " << t.quantity << "\n";
    }
}

int main() {
    OrderBook book;

    // adding seed 1 resting book updated (bids | asks)
    book.add_order(1001, 10050, 100, Side::Buy,  OrderType::Limit);
    book.add_order(1002, 10048, 150, Side::Buy,  OrderType::Limit);
    book.add_order(2001, 10052, 100, Side::Sell, OrderType::Limit);
    book.add_order(2002, 10055, 200, Side::Sell, OrderType::Limit);

    std::cout << "=== INITIAL BOOK ===\n";
    book.print_book();
    
    // FOK  buy that should be killed (only 100 available <= 10052, need 150)
    auto t1 = book.add_order(3001, 10052, 150, Side::Buy, OrderType::FOK);
    print_trades("FOK Buy 150 @ 10052 (Expect Kill)", t1);

    // IOC buy partially fills 100 @ 10052 and cancels remaining 50
    auto t2 = book.add_order(3002, 10052, 150, Side::Buy, OrderType::IOC);
    print_trades("IOC Buy 150 @ 10052 (Expect 100 Fill)", t2);

    // agresive limit sell sweeping across both bid levels (100 @ 10050, 50 @ 10048)
    auto t3 = book.add_order(3003, 10048, 150, Side::Sell, OrderType::Limit);
    print_trades("Limit Sell 150 @ 10048 (Expect 2 Fills)", t3);

    // market buy sweeping 120 from the remaining 200 @ 10055 ask
    auto t4 = book.add_order(3004, 0, 120, Side::Buy, OrderType::Market);
    print_trades("Market Buy 120 (Expect 120 @ 10055)", t4);

    std::cout << "\n=== FINAL BOOK ===\n";
    book.print_book();

    return 0;
}
