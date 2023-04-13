/*
 * itch.h
 *
 *  Created on: May 24, 2018
 *      Author: Ali Volkan ATLI
 */

#ifndef _ITCH_H_
#define _ITCH_H_

/*
 * This application is written according to following documents:
 *
 * 1) Borsa İstanbul ITCH Protocol Specification Rules of Engagement
 *    December 30, 2014 Version 1.2
 *
 * 2) Genium  INET (R) ITCH Protocol Specification BIST
 *    February 28, 2017 Version 4.0.1024
 *
 * The ITCH feed is made up of a series of sequenced messages.
 * Each message is variable in length based on the message type.
 * The messages will be binary encoded using MoldUDP64.
 *
 * This application also takes care of sequencing and delivery guarantees
 *
 * Note: MoldUDP64 uses big-endian for its numeric values
 */

#include <inttypes.h>
/*
 * Every ITCH Packet
 * Downstream Header (20 bytes) + Message Blocks (variable bytes) ==> max 1024 byte
 */
typedef struct __attribute__((packed)) _DownstreamHeader {
    char session[10];               // Not used by Borsa İstanbul
    uint64_t sequence_number;
    uint16_t message_count;
    uint8_t data[0];
} DownstreamHeader;


struct __attribute__((packed)) MessageBlock {
    uint16_t lenght;
    uint8_t data[0];
};

struct __attribute__((packed)) Heartbeat {
    uint8_t type;
    uint32_t second;
    uint32_t nanoseconds;
    uint64_t last_seq_num;
};

/*      Message Mapping
 *      ---------------------------
 * "T"  Seconds
 * "R"  Order Book Directory
 * "L"  Tick Size Table Entry
 * "S"  System Event
 * "O"  Order Book State
 * "A"  Add Order
 * "E"  Order Executed
 * "C"  Order Executed with Price
 * "D"  Order Delete
 * "P"  Trade
 * "Z"  Equilibrium Price Update
 * "M"  Combination Order Book Leg
 *
 * */

/* Ref1 - Ch 2.3.2.1 Seconds message */
struct __attribute__((packed)) Seconds {
    uint8_t type;                   // "T" mapping
    uint32_t second;
};

/* Ref1 - Ch 2.3.3.1 Order Book Directory message */
struct __attribute__((packed)) OrderBookDirectory {
    uint8_t type;                   // "R" mapping
    uint32_t nanoseconds;
    uint32_t order_book_id;         // Denotes the primary identifier of an order book
    char symbol[32];                // Security short name
    char long_name[32];             // Human-readable long name of security
    char isin[12];                  // ISIN code identifying security
    uint8_t financial_product;      // Default value is 5 (for Cash)
    char trading_currency[3];       // Default value is "TRY"
    uint16_t decimals;              // Number of decimals in Price, default value is 3
    uint16_t nominal;               // Number of decimals in Nominal Value
    uint32_t reserved1;             // Not used by Borsa İstanbul
    uint32_t reserved2;             // Not used by Borsa İstanbul
    uint32_t reserved3;             // Not used by Borsa İstanbul
    uint64_t nominal_value;         // Not used by Borsa İstanbul
    char reserved[16];
};

/* Ref1 - Ch 2.3.3.2 Tick Size Table Entry message */
struct __attribute__((packed)) TickSizeTableEntry {
    uint8_t type;                   // "L" mapping
    uint32_t nanoseconds;
    uint32_t order_book_id;
    int64_t tick_size;             // Tick Size for the given price range
    int32_t price_from;            // Start of price range for this entry
    int32_t price_to;              // End of price range for this entry
};

/* Ref1 - Ch 2.3.4.1 System Event message */
struct __attribute__((packed)) SystemEvent {
    uint8_t type;                   // "S" mapping
    uint32_t nanoseconds;
    uint8_t event_code;             // O = start of messages # C = end of messages
};

/* Ref1 - Ch 2.3.4.2 Order Book State message */
struct __attribute__((packed)) OrderBookState {
    uint8_t type;                   // "O" mapping
    uint32_t nanoseconds;
    uint32_t order_book_id;
    char state_name[20];            // O, C, S, Q, U, please check get_trading_status()
};

struct __attribute__((packed)) OrderCommon {
    uint8_t type;                   // for "ADE"
    uint32_t nanoseconds;
    uint64_t order_id;              // The unique identifier assigned to the new order
    uint32_t order_book_id;
};

/* Ref1 - Ch 2.3.5.1 Add Order message */
struct __attribute__((packed)) OrderAdd {
    uint8_t type;                   // "A" mapping
    uint32_t nanoseconds;
    uint64_t order_id;              // The unique identifier assigned to the new order
    uint32_t order_book_id;
    uint8_t side;                   // Type of order, "B" = buy order and "S" = sell order
    uint32_t order_book_position;   // Rank within order book
    uint64_t quantity;              // The quantity of the order
    int32_t price;                  // The price of the new order, prices are signed integer fields.
    uint16_t quote_indicator;
    uint8_t lot_type;
};

/* Ref1 - Ch 2.3.5.2.1 Order Executed message */
struct __attribute__((packed)) OrderExecuted {
    uint8_t type;                   // "E" mapping
    uint32_t nanoseconds;
    uint64_t order_id;
    uint32_t order_book_id;
    uint8_t side;
    uint64_t quantity;
    uint64_t match_id;              // Assigned by the system to each match executed
    uint32_t combo_group_id;        // Not used by Borsa İstanbul
    char reserved1[7];              // Not used by Borsa İstanbul
    char reserved2[7];              // Not used by Borsa İstanbul
};

/* Ref1 - Ch 2.3.5.2.3 Order Delete message */
struct __attribute__((packed)) OrderDelete {
    uint8_t type;                   // "D" mapping
    uint32_t nanoseconds;
    uint64_t order_id;
    uint32_t order_book_id;
    uint8_t side;
};

/* Ref1 - Ch 2.3.6 Trade message */
struct __attribute__((packed)) Trade {
    uint8_t type;                   // "P" mapping
    uint32_t nanoseconds;
    uint64_t match_id;
    uint32_t combo_group_id;        // Not used by Borsa İstanbul
    uint8_t side;
    uint64_t quantity;
    uint32_t order_book_id;
    int32_t trade_price;
    char reserved1[7];
    char reserved2[7];
    uint8_t printable;
    uint8_t occurred_at_cross;
};

/* Ref2 - Ch 4.4.2.2 Order Executed with Price Message */
struct __attribute__((packed)) OrderExecutedWithPrice {
    uint8_t type;                   // "C" mapping
    uint32_t nanoseconds;
    uint64_t order_id;
    uint32_t order_book_id;
    uint8_t side;
    uint64_t quantity;
    uint64_t match_id;
    uint32_t combo_group_id;
    char reserved1[7];
    char reserved2[7];
    int32_t trade_price;
    uint8_t occurred_at_cross;
    uint8_t printable;
};

/* Ref2 - Ch 4.5.2.1 Equilibrium Price Update Message */
struct __attribute__((packed)) EquilibriumPriceUpdate {
    uint8_t type;                   // "Z" mapping
    uint32_t nanoseconds;
    uint32_t order_book_id;
    uint64_t available_bid_quantity;
    uint64_t available_ask_quantity;
    int32_t equilibrium_price;
    int32_t best_bid_price;
    int32_t best_ask_price;
    uint64_t best_bid_quantity;
    uint64_t best_ask_quantity;
};

/* Ref2 - Ch 4.2.2 Order Executed with Price Message */
struct __attribute__((packed)) CombinationOrderBookLeg {
    uint8_t type;                   // "M" mapping
    uint32_t nanoseconds;
    uint32_t combination_order_book_id;
    uint32_t leg_order_book_id;
    uint8_t leg_side;
    uint32_t leg_ratio;
};

#endif /* _ITCH_H_ */
