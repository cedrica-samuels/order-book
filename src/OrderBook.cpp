#include "OrderBook.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>

namespace {

template <typename Book>
int totalAtLevel(const Book& book, int price) {
    auto it = book.find(price);
    if (it == book.end()) {
        return 0;
    }
    int total = 0;
    for (const Order& order : it->second) {
        total += order.quantity;
    }
    return total;
}

void printLevel(int price, const std::deque<Order>& queue) {
    int total = 0;
    for (const Order& order : queue) {
        total += order.quantity;
    }
    std::cout << std::setw(8) << price << std::setw(10) << total << "\n";
}

template <typename Book, typename Locations>
void matchAgainst(Book& book, Order& incoming,
                  Locations& locations,
                  std::vector<Trade>& trades) {
    bool incomingIsBuy = incoming.side == Side::Buy;

    while (incoming.quantity > 0 && !book.empty()) {
        auto level = book.begin();
        int levelPrice = level->first;

        bool crosses = incomingIsBuy ? levelPrice <= incoming.price
                                     : levelPrice >= incoming.price;
        if (!crosses) {
            break;
        }

        auto& queue = level->second;
        while (incoming.quantity > 0 && !queue.empty()) {
            Order& resting = queue.front();
            int fillQty = std::min(incoming.quantity, resting.quantity);

            Trade trade;
            trade.buyId = incomingIsBuy ? incoming.id : resting.id;
            trade.sellId = incomingIsBuy ? resting.id : incoming.id;
            trade.price = levelPrice;
            trade.quantity = fillQty;
            trades.push_back(trade);

            incoming.quantity -= fillQty;
            resting.quantity -= fillQty;

            if (resting.quantity == 0) {
                locations.erase(resting.id);
                queue.pop_front();
            }
        }

        if (queue.empty()) {
            book.erase(level);
        }
    }
}

template <typename Book>
bool removeFromLevel(Book& book, int price, int id) {
    auto level = book.find(price);
    if (level == book.end()) {
        return false;
    }
    auto& queue = level->second;
    for (auto it = queue.begin(); it != queue.end(); ++it) {
        if (it->id == id) {
            queue.erase(it);
            if (queue.empty()) {
                book.erase(level);
            }
            return true;
        }
    }
    return false;
}

}

std::vector<Trade> OrderBook::addOrder(int id, Side side, int price, int quantity) {
    std::vector<Trade> trades;

    if (price <= 0 || quantity <= 0 || locations.count(id) > 0) {
        return trades;
    }

    Order order{id, side, price, quantity, nextSequence++};

    if (side == Side::Buy) {
        matchAgainst(asks, order, locations, trades);
    } else {
        matchAgainst(bids, order, locations, trades);
    }

    if (order.quantity > 0) {
        if (side == Side::Buy) {
            bids[price].push_back(order);
        } else {
            asks[price].push_back(order);
        }
        locations[id] = Location{side, price};
    }

    return trades;
}

bool OrderBook::cancelOrder(int id) {
    auto found = locations.find(id);
    if (found == locations.end()) {
        return false;
    }

    Location where = found->second;
    bool removed;
    if (where.side == Side::Buy) {
        removed = removeFromLevel(bids, where.price, id);
    } else {
        removed = removeFromLevel(asks, where.price, id);
    }

    locations.erase(found);
    return removed;
}

std::optional<int> OrderBook::bestBid() const {
    if (bids.empty()) {
        return std::nullopt;
    }
    return bids.begin()->first;
}

std::optional<int> OrderBook::bestAsk() const {
    if (asks.empty()) {
        return std::nullopt;
    }
    return asks.begin()->first;
}

std::optional<int> OrderBook::spread() const {
    if (bids.empty() || asks.empty()) {
        return std::nullopt;
    }
    return asks.begin()->first - bids.begin()->first;
}

int OrderBook::quantityAt(Side side, int price) const {
    if (side == Side::Buy) {
        return totalAtLevel(bids, price);
    }
    return totalAtLevel(asks, price);
}

void OrderBook::printBook() const {
    std::cout << "---- ORDER BOOK ----\n";
    std::cout << std::setw(8) << "Price" << std::setw(10) << "Qty" << "\n";

    std::cout << "ASKS\n";
    if (asks.empty()) {
        std::cout << "  (empty)\n";
    }
    for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
        int total = 0;
        for (const Order& order : it->second) {
            total += order.quantity;
        }
        std::cout << std::setw(8) << it->first << std::setw(10) << total << "\n";
    }

    std::cout << "BIDS\n";
    if (bids.empty()) {
        std::cout << "  (empty)\n";
    }
    for (const auto& level : bids) {
        printLevel(level.first, level.second);
    }
    std::cout << "--------------------\n";
}
