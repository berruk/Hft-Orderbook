
//WRITTEN

#ifndef LL_ORDERBOOK_H
#define LL_ORDERBOOK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "pcap.h"


#define SIZE_HASHMAP 80000
#define SIDE 2

struct order
{
    uint64_t order_id;
    uint32_t price;
    uint64_t quantity;
    uint8_t filled;

};

struct OrderBook 
{
    std::list<std::pair<int, uint64_t>> buy_side;
    std::list<std::pair<int, uint64_t>> sell_side;

    int buy_price;
    int sell_price;  
    uint32_t order_book_id;
    struct order orders[SIZE_HASHMAP][SIDE];
};

extern struct OrderBook order_book;

int get_buy_head_price(void);

int get_sell_head_price(void);

bool order_book_is_not_empty(void);

void log_price_changes(struct timeval *ts);

void initialize_order_book(void);

void insert_to_buy_side(int price, uint64_t quantity);

void insert_to_sell_side(int price, uint64_t quantity);

void decrease_quantity_of_price_buy_side(int price, uint64_t quantity);

void decrease_quantity_of_price_sell_side(int price, uint64_t quantity);

void print_sell(void);

void print_buy(void);

#endif