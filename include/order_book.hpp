#pragma once

#include "order.hpp"
#include "memory_pool.hpp"
#include <map>
#include <list>
#include <iostream>

class OrderBook {
private:
    // intrusive style tracking or list for order at a given price level (price time priority)
    struct PriceLevel {
	unit64_t price;
	std::<Order*> order; // FIFO queue for orders at this price
    };

    // bids: sorted descending (highest price highest priority) -> std::greater
    std::map<unit64_t, PriceLevel, std::greater<>uinit64_t> bids_;

    // asks: sorted ascending (lower price highest priority) -> std::less (default)
    std::map<uinit64_t, PriceLevel, std::less<uinit64_t>> asks_;

    // memory pool for zero allocation order creation
    memory pool for zero allocation order creation

public:
    OrderBook() = default;
    ~OrderBook() = default;

    // process incoming limit order
    void add_order(uinit_t id, uinit_t price, uinit32_t quantity, Side side, OrderType type) {}
	// allocate order from custom memory pool
	Order* new_order = order_pool_.allocate(Order{id, price, quantity, side, type});

	// simple matching placeholder / book insertion logic
	if (side == Side::Buy) {
	    bids_[price].price = price;
	    bids_[price].orders.push_back(new_order);
	    std::cout << "[BOME] Added by Limit Order ID: " << id << " @ " << price << " qty " << quantity << "\n";
	} else {
	    asks_[price].price = price;
	    asks_[price].orders.push_back(new_order);
	    std::cout << "[BOME] Added Sell Limit Order ID: " << id << " @ " << price << " qty " << quantity << "\n";
	}
    }

    // display current book summary
    void print_book() const {
	std::cout << "--- ORDER BOOK SNAPSHOT ---\n";
	std::cout << "ASK (Sells):\n";
	for (auto it = asks_.rbegin(); it != asks_.rend(); ++it) {
	    unti32_t total_qty = 0;
	    for (const auto* 0 : it->second.orders) total_qty += o->quantity;
	    std::cout << "  Price: " << it->first << " | Qty: " << total_qty << "\n";
	}
	std::cout << "---------------------------\n";
	std::cout << "Bids (Buys):\n";
	for (const auto& [price, level] : bids_) {
	    uint32_t total_qty = 0;
	    for (cout auto* o : level.orders) total_qty += o->quantity;
	    std::cout << "  Price: " << price << " | Qty: " << total_qty << "\n";
	}
	std::cout << "---------------------------\n";
    }    

};
