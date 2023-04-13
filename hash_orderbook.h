//WRITTEN

#ifndef HASH_ORDERBOOK_H
#define HASH_ORDERBOOK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "hashmap.h"

#define SIZE_HASHMAP 80000
#define SIDE 2

struct order
{
    uint64_t order_id;
    uint32_t price;
    uint64_t quantity;
    uint8_t filled;

};

struct lessmset
{
    bool operator()(const std::pair<int, uint64_t> &p1, const std::pair<int, uint64_t> &p2)
    {
        return p1.first < p2.first;
    }
};

struct grmset
{
    bool operator()(const std::pair<int, uint64_t> &p1, const std::pair<int, uint64_t> &p2)
    {
        return p1.first > p2.first;
    }
};
struct OrderBook 
{
    std::multiset<std::pair<int, uint64_t>, grmset> buy_side;
    std::multiset<std::pair<int, uint64_t>, grmset> sell_side;
    int sell_price;
    int buy_price; 
    uint32_t order_book_id;
    struct order orders[SIZE_HASHMAP][SIDE];
};

extern struct OrderBook order_book;

bool order_book_is_not_empty(void);

void log_price_changes(struct timeval *ts);

int get_buy_head_price(void);

int get_sell_head_price(void);

void insert_to_buy_side(int price, uint64_t quantity);

void insert_to_sell_side(int price, uint64_t quantity);

void decrease_quantity_of_price_buy_side(int price, uint64_t quantity);

void decrease_quantity_of_price_sell_side(int price, uint64_t quantity);

void print_sell(void);

void print_buy(void);

#endif