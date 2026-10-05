# Order Book

A limit order book and matching engine in C++17, using only the standard library.

## Build, run, test

    g++ -std=c++17 -Wall -Wextra -o orderbook src/main.cpp
    ./orderbook

    g++ -std=c++17 -Wall -Wextra -I src tests/test_orderbook.cpp src/OrderBook.cpp -o tests
    ./tests

`main.cpp` includes `OrderBook.cpp` directly so the one-file build command works.

## Design choices

- `std::map` of price to `std::deque`: the map keeps price levels sorted so the best price is always at `begin()`, and the deque holds the orders at that price in arrival order (FIFO).
- Bids use `std::greater<int>` so the highest bid comes first. Asks use the default order so the lowest ask comes first.
- Prices are integer ticks, not `double`, because floating point can't represent values like 0.1 exactly and comparisons between prices can go wrong.
- An `unordered_map` from order id to side and price lets `cancelOrder` jump straight to the right price level.

## What is covered

Level 1: orders, both sides of the book, `addOrder`, `cancelOrder`, `bestBid`, `bestAsk`, `spread`, `printBook`, empty-side handling.

Level 2: matching on add, price-time priority, trades at the resting order's price, partial fills, removal of filled orders and empty levels, a `Trade` returned for each fill.

Invalid orders (price or quantity <= 0, or a duplicate id) are ignored.

## Possible extensions (NOT implemented)

- Market orders
- A market maker
- A simulation with random order flow
- Faster cancels (currently a scan within one price level)
