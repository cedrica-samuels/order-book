#include <iostream>
#include <string>

#include "OrderBook.hpp"

int failures = 0;

void check(bool condition, const std::string& message) {
    if (!condition) {
        std::cout << "    check failed: " << message << "\n";
        failures++;
    }
}

void test1() {
    OrderBook book;
    book.addOrder(1, Side::Buy, 99, 10);
    book.addOrder(2, Side::Buy, 100, 5);
    book.addOrder(3, Side::Sell, 102, 7);
    book.addOrder(4, Side::Sell, 103, 3);
    check(book.bestBid() == 100, "best bid is 100");
    check(book.bestAsk() == 102, "best ask is 102");
    check(book.spread() == 2, "spread is 2");
}

void test2() {
    OrderBook book;
    book.addOrder(1, Side::Sell, 100, 10);
    auto trades = book.addOrder(2, Side::Buy, 100, 10);
    check(trades.size() == 1, "one trade");
    check(trades[0].buyId == 2 && trades[0].sellId == 1, "buyer 2, seller 1");
    check(trades[0].price == 100 && trades[0].quantity == 10, "100 x 10");
    check(!book.bestAsk().has_value(), "ask side empty");
    check(!book.bestBid().has_value(), "bid side empty");
}

void test3() {
    OrderBook book;
    book.addOrder(1, Side::Sell, 100, 4);
    auto trades = book.addOrder(2, Side::Buy, 100, 10);
    check(trades.size() == 1 && trades[0].quantity == 4, "filled 4");
    check(!book.bestAsk().has_value(), "ask side empty");
    check(book.bestBid() == 100, "remainder rests as bid at 100");
    check(book.quantityAt(Side::Buy, 100) == 6, "6 left resting");
}

void test4() {
    OrderBook book;
    book.addOrder(1, Side::Sell, 100, 10);
    book.addOrder(2, Side::Sell, 100, 5);
    auto first = book.addOrder(3, Side::Buy, 100, 4);
    check(first.size() == 1 && first[0].sellId == 1 && first[0].quantity == 4, "order 1 filled 4");
    check(book.quantityAt(Side::Sell, 100) == 11, "10 - 4 + 5 = 11 left");
    auto second = book.addOrder(4, Side::Buy, 100, 6);
    check(second.size() == 1 && second[0].sellId == 1 && second[0].quantity == 6, "order 1 still first in line");
    check(book.quantityAt(Side::Sell, 100) == 5, "only order 2 left");
}

void test5() {
    OrderBook book;
    book.addOrder(1, Side::Sell, 100, 5);
    book.addOrder(2, Side::Sell, 101, 5);
    book.addOrder(3, Side::Sell, 102, 5);
    auto trades = book.addOrder(4, Side::Buy, 102, 12);
    check(trades.size() == 3, "three trades");
    check(trades[0].price == 100 && trades[0].quantity == 5, "level 100 first");
    check(trades[1].price == 101 && trades[1].quantity == 5, "level 101 second");
    check(trades[2].price == 102 && trades[2].quantity == 2, "level 102 partly");
    check(book.bestAsk() == 102, "102 remains");
    check(book.quantityAt(Side::Sell, 102) == 3, "3 left at 102");
    check(!book.bestBid().has_value(), "nothing rests on bid side");
}

void test6() {
    OrderBook book;
    book.addOrder(1, Side::Buy, 100, 5);
    book.addOrder(2, Side::Buy, 100, 5);
    auto trades = book.addOrder(3, Side::Sell, 100, 5);
    check(trades.size() == 1 && trades[0].buyId == 1, "older order 1 filled first");
    auto more = book.addOrder(4, Side::Sell, 100, 5);
    check(more.size() == 1 && more[0].buyId == 2, "then order 2");
}

void test7() {
    OrderBook book;
    book.addOrder(1, Side::Sell, 100, 5);
    auto trades = book.addOrder(2, Side::Buy, 105, 5);
    check(trades.size() == 1 && trades[0].price == 100, "buy at 105 trades at 100");

    book.addOrder(3, Side::Buy, 98, 5);
    auto sellTrades = book.addOrder(4, Side::Sell, 95, 5);
    check(sellTrades.size() == 1 && sellTrades[0].price == 98, "sell at 95 trades at 98");
}

void test8() {
    OrderBook book;
    book.addOrder(1, Side::Buy, 100, 5);
    book.addOrder(2, Side::Sell, 105, 5);
    check(book.cancelOrder(1), "cancel of order 1 works");
    check(!book.bestBid().has_value(), "bid side empty after cancel");
    check(!book.cancelOrder(1), "cancelling again returns false");
    check(!book.cancelOrder(999), "unknown id returns false");
    check(book.bestAsk() == 105, "other side untouched");
    auto trades = book.addOrder(3, Side::Buy, 100, 5);
    check(trades.empty(), "cancelled order does not trade");
}

void test9() {
    OrderBook book;
    check(!book.bestBid().has_value(), "no best bid");
    check(!book.bestAsk().has_value(), "no best ask");
    check(!book.spread().has_value(), "no spread");
    check(!book.cancelOrder(1), "cancel on empty book");
    book.printBook();
    book.addOrder(1, Side::Buy, 100, 5);
    check(!book.spread().has_value(), "no spread with one side only");
}

void run(int number, void (*test)()) {
    int before = failures;
    test();
    std::cout << "Test " << number << ": " << (failures == before ? "PASS" : "FAIL") << "\n";
}

int main() {
    run(1, test1);
    run(2, test2);
    run(3, test3);
    run(4, test4);
    run(5, test5);
    run(6, test6);
    run(7, test7);
    run(8, test8);
    run(9, test9);
    std::cout << failures << " failed check(s)\n";
    return failures == 0 ? 0 : 1;
}
