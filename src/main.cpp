#include <iostream>
#include <string>
#include <vector>

#include "OrderBook.cpp"

void printTrades(const std::vector<Trade>& trades) {
    if (trades.empty()) {
        std::cout << "No trades\n";
        return;
    }
    for (const Trade& t : trades) {
        std::cout << "TRADE buyId=" << t.buyId << " sellId=" << t.sellId
                  << " price=" << t.price << " qty=" << t.quantity << "\n";
    }
}

void step(const std::string& text, OrderBook& book, const std::vector<Trade>& trades) {
    std::cout << "\n>>> " << text << "\n";
    printTrades(trades);
    book.printBook();
}

int main() {
    OrderBook book;

    step("Buy 10 @ 100 (id 1)", book, book.addOrder(1, Side::Buy, 100, 10));
    step("Buy 5 @ 99 (id 2)", book, book.addOrder(2, Side::Buy, 99, 5));
    step("Sell 8 @ 103 (id 3)", book, book.addOrder(3, Side::Sell, 103, 8));
    step("Sell 6 @ 102 (id 4)", book, book.addOrder(4, Side::Sell, 102, 6));
    step("Sell 4 @ 100 (id 5) crosses the best bid", book, book.addOrder(5, Side::Sell, 100, 4));
    step("Buy 12 @ 103 (id 6) sweeps two ask levels", book, book.addOrder(6, Side::Buy, 103, 12));

    std::cout << "\n>>> Cancel id 2: " << (book.cancelOrder(2) ? "done" : "not found") << "\n";
    book.printBook();

    std::cout << "\n>>> Cancel id 99: " << (book.cancelOrder(99) ? "done" : "not found") << "\n";
    return 0;
}
