// #include <concepts>
#include "order_book.hpp"
#include <iostream>

int main() {
    std::cout << "Starting BOME (Order Book Matching Engine) Test Harness...\n\n";

    OrderBook book;

    // adding test buy orders (bids)
    book.add_order(1001, 10050, 100, Side::Buy, OrderType::Limit);
    book.add_order(1002, 10050, 200, Side::Buy, OrderType::Limit);
    book.add_order(1003, 10048, 150, Side::Buy, OrderType::Limit);

    // adding test sell orders (asks)
    book.add_order(1001, 10050, 100, Side::Sell, OrderType::Limit);
    book.add_order(1001, 10050, 100, Side::Sell, OrderType::Limit);

    std::cout << "\n";
    book.print_book();

    std::cout << "\nBOME Test Harness Complete!\n";
    return 0;
}
