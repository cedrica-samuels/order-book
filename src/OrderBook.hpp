#pragma once

#include <deque>
#include <functional>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

#include "Order.hpp"

class OrderBook {
public:
    std::vector<Trade> addOrder(int id, Side side, int price, int quantity);
    bool cancelOrder(int id);

    std::optional<int> bestBid() const;
    std::optional<int> bestAsk() const;
    std::optional<int> spread() const;
    int quantityAt(Side side, int price) const;

    void printBook() const;

private:
    struct Location {
        Side side;
        int price;
    };

    std::map<int, std::deque<Order>, std::greater<int>> bids;
    std::map<int, std::deque<Order>> asks;
    std::unordered_map<int, Location> locations;
    int nextSequence = 1;
};
