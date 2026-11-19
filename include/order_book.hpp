#pragma once

#include "order.hpp"
#include "memory_pool.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <map>

class OrderBook {
private:
    // intrusive style tracking or list for order at a given price level (p>
    struct PriceLevel {
        uint64_t price = 0;
        std::list<Order*> orders; // FIFO queue for orders at this price
    };

    // bids: highest price first | Asks: lowest price first
    std::map<uint64_t, PriceLevel, std::greater<uint64_t>> bids_;
    std::map<uint64_t, PriceLevel, std::less<uint64_t>> asks_;

    MemoryPool<Order> order_pool_;

    // dry run liquidity check for kill | fill (FOK) orders
    template <typename BookType, typename PriceMatchFn>
    bool can_fill(const BookType& opposite_book, uint32_t quantity, PriceMatchFn price_matches) const {
        uint32_t available = 0;
        for (const auto& [level_price, level] : opposite_book) {
            if (!price_matches(level_price)) break;
            for (const Order* resting : level.orders) {
                available += resting->quantity;
                if (available >= quantity) return true;
            }
        }
        return false;
    }

    // core price time priority matching loop
    template <typename BookType, typename PriceMatchFn>
    void match_against(uint64_t incoming_id, Side incoming_side, uint32_t& remaining_qty,
                       BookType& opposite_book, PriceMatchFn price_matches,
                       std::vector<Trade>& trades) {
        auto level_it = opposite_book.begin();

        while (remaining_qty > 0 && level_it != opposite_book.end()) {
            uint64_t level_price = level_it->first;
            if (!price_matches(level_price)) break;

            auto& orders = level_it->second.orders;
            auto order_it = orders.begin();

            while (remaining_qty > 0 && order_it != orders.end()) {
                Order* resting = *order_it;
                uint32_t fill_qty = std::min(remaining_qty, resting->quantity);

                uint64_t buy_id  = (incoming_side == Side::Buy) ? incoming_id : resting->order_id;
                uint64_t sell_id = (incoming_side == Side::Sell) ? incoming_id : resting->order_id;

                trades.push_back(Trade{buy_id, sell_id, level_price, fill_qty});

                remaining_qty -= fill_qty;
                resting->quantity -= fill_qty;

                if (resting->quantity == 0) {
                    order_pool_.deallocate(resting);
                    order_it = orders.erase(order_it);
                } else {
                    ++order_it;
                }
            }

            if (orders.empty()) {
                level_it = opposite_book.erase(level_it);
            } else {
                ++level_it;
            }
        }
    }

public:
    OrderBook() = default;
    ~OrderBook() = default;

    // process incoming trade order vector
    std::vector<Trade> add_order(uint64_t id, uint64_t price, uint32_t quantity,
                                 Side side, OrderType type) {
        std::vector<Trade> trades;
        if (quantity == 0) return trades;

        uint32_t remaining_qty = quantity;

        if (side == Side::Buy) {
            auto price_ok = [price, type](uint64_t ask_price) {
                return type == OrderType::Market || ask_price <= price;
            };

            if (type == OrderType::FOK && !can_fill(asks_, remaining_qty, price_ok)) {
                return trades; // killed: insufficient liquidity
            }

            match_against(id, Side::Buy, remaining_qty, asks_, price_ok, trades);

            if (remaining_qty > 0 && type == OrderType::Limit) {
                Order* resting = order_pool_.allocate(Order{id, price, remaining_qty, side, type});
                bids_[price].price = price;
                bids_[price].orders.push_back(resting);
            }
        } else {
            auto price_ok = [price, type](uint64_t bid_price) {
                return type == OrderType::Market || bid_price >= price;
            };

            if (type == OrderType::FOK && !can_fill(bids_, remaining_qty, price_ok)) {
                return trades; // killed: insufficient liquidity
            }

            match_against(id, Side::Sell, remaining_qty, bids_, price_ok, trades);

            if (remaining_qty > 0 && type == OrderType::Limit) {
                Order* resting = order_pool_.allocate(Order{id, price, remaining_qty, side, type});
                asks_[price].price = price;
                asks_[price].orders.push_back(resting);
            }
        }

        return trades;
    }

    // display current book summary
    void print_book() const {
        std::cout << "--- ORDER BOOK SNAPSHOT ---\n";
        std::cout << "ASKS (Sells):\n";
        for (auto it = asks_.rbegin(); it != asks_.rend(); ++it) {
            uint32_t total_qty = 0;
            for (const auto* o : it->second.orders) total_qty += o->quantity;
            std::cout << "  Price: " << it->first << " | Qty: " << total_qty << "\n";
        }
        std::cout << "---------------------------\n";
        std::cout << "BIDS (Buys):\n";
        for (const auto& [price, level] : bids_) {
            uint32_t total_qty = 0;
            for (const auto* o : level.orders) total_qty += o->quantity;
            std::cout << "  Price: " << price << " | Qty: " << total_qty << "\n";
        }
        std::cout << "---------------------------\n";
    }
};
