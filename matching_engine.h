#pragma once
#include "order_book.h"
#include "spsc_queue.h"
#include<atomic>
#include <cstdint>
#include <limits>
class MatchingEngine{
public:
 MatchingEngine(SPSCQueue<Order,1024>& q);
 void run();
  void stop();

private:
   SPSCQueue<Order,1024>& queue;
    OrderBook book;
   std::atomic<bool> running{true};
   //latency parameters
    uint64_t min_latency{std::numeric_limits<uint64_t>::max()};
    uint64_t max_latency{0};
    uint64_t total_latency{0};
    uint64_t count{0};

};