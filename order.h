#pragma once
#include <cstdint>
enum class Side {
    Buy,
    Sell
};
struct Order {
    uint64_t id;
    Side side;
    int price;
    int quantity;
    uint64_t timestamp;  // for time priority
};