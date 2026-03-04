#pragma once
#include <cstdint>
enum class Side{
  Sell,
   Buy
};

struct Order{  
  uint64_t id;
  int price;
  Side side;
  int quantity;
   uint64_t timestamp;
};