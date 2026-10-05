#pragma once

enum class Side { Buy, Sell };

struct Order {
    int id;
    Side side;
    int price;
    int quantity;
    int sequence;
};

struct Trade {
    int buyId;
    int sellId;
    int price;
    int quantity;
};
