#pragma once

#include <cstdint>

enum class Side : uint8_t {
    Buy = 0,
    Sell = 1
};

enum class OrderType : uint8_t {
    Limit = 0,
    Market = 1
};

struct Order {
    uint64_t order_id;     // 8 bytes - Unique order identifier
    uint64_t price;        // 8 bytes - Limit price (in ticks/smallest currency unit)
    uint32_t quantity;     // 4 bytes - Number of shares/units
    Side side;             // 1 byte  - Buy or Sell
    OrderType type;        // 1 byte  - Limit or Market
    // 2 bytes of structural padding to maintain 8-byte alignment (Total size: 24 bytes)
};
