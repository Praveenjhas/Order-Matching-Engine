#pragma once
#include "order_book.h"
#include "spsc_queue.h"
#include "order.h"
#include <atomic>

class MatchingEngine {
public:
    MatchingEngine(SPSCQueue<Order, 1024>& q);
    void run();
    void stop();

private:
    SPSCQueue<Order, 1024>& queue;
    OrderBook book;
    std::atomic<bool> running{true};
};