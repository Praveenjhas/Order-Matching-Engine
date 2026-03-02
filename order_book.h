#pragma once
#include <map>
#include <deque>
#include <unordered_map>
#include <iostream>
#include "order.h"

class OrderBook {
private:
    // Buy: highest price first
    std::map<int, std::deque<Order>, std::greater<int>> buy_book;

    // Sell: lowest price first
    std::map<int, std::deque<Order>> sell_book;

    // Order ID lookup for cancellation
    struct OrderLocation {
        Side side;
        int price;
        std::deque<Order>::iterator it;
    };

    std::unordered_map<uint64_t, OrderLocation> order_index;

public:
    void add_order(Order order) {
        if (order.side == Side::Buy) {
            match_buy(order);
            if (order.quantity > 0) {
                auto& level = buy_book[order.price];
                level.push_back(order);
                auto it = std::prev(level.end());
                order_index[order.id] = {Side::Buy, order.price, it};
            }
        } else {
            match_sell(order);
            if (order.quantity > 0) {
                auto& level = sell_book[order.price];
                level.push_back(order);
                auto it = std::prev(level.end());
                order_index[order.id] = {Side::Sell, order.price, it};
            }
        }
    }

    void cancel_order(uint64_t order_id) {
        auto found = order_index.find(order_id);
        if (found == order_index.end()) return;

        auto loc = found->second;

        if (loc.side == Side::Buy) {
            auto& level = buy_book[loc.price];
            level.erase(loc.it);
            if (level.empty()) {
                buy_book.erase(loc.price);
            }
        } else {
            auto& level = sell_book[loc.price];
            level.erase(loc.it);
            if (level.empty()) {
                sell_book.erase(loc.price);
            }
        }

        order_index.erase(order_id);
    }

private:
    void match_buy(Order& incoming) {
        while (incoming.quantity > 0 && !sell_book.empty()) {
            auto best_sell_it = sell_book.begin();  // lowest sell price
            int best_price = best_sell_it->first;

            if (best_price > incoming.price)
                break;  // no price match

            auto& level = best_sell_it->second;

            while (incoming.quantity > 0 && !level.empty()) {
                Order& resting = level.front();

                int traded_qty = std::min(incoming.quantity, resting.quantity);

                std::cout << "Trade: BUY " << incoming.id
                          << " with SELL " << resting.id
                          << " @ " << best_price
                          << " qty " << traded_qty << "\n";

                incoming.quantity -= traded_qty;
                resting.quantity -= traded_qty;

                if (resting.quantity == 0) {
                    order_index.erase(resting.id);
                    level.pop_front();
                }
            }

            if (level.empty()) {
                sell_book.erase(best_price);
            }
        }
    }

    void match_sell(Order& incoming) {
        while (incoming.quantity > 0 && !buy_book.empty()) {
            auto best_buy_it = buy_book.begin();  // highest buy price
            int best_price = best_buy_it->first;

            if (best_price < incoming.price)
                break;  // no price match

            auto& level = best_buy_it->second;

            while (incoming.quantity > 0 && !level.empty()) {
                Order& resting = level.front();

                int traded_qty = std::min(incoming.quantity, resting.quantity);

                std::cout << "Trade: SELL " << incoming.id
                          << " with BUY " << resting.id
                          << " @ " << best_price
                          << " qty " << traded_qty << "\n";

                incoming.quantity -= traded_qty;
                resting.quantity -= traded_qty;

                if (resting.quantity == 0) {
                    order_index.erase(resting.id);
                    level.pop_front();
                }
            }

            if (level.empty()) {
                buy_book.erase(best_price);
            }
        }
    }
};