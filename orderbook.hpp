
#ifndef _ORDERBOOK_H
#define _ORDERBOOK_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <utility>
#include <map>
#include <set>
#include <vector>
#include <list>

#define INPUT_ITCH_DIRECTORY "itch.pcap"
#define INSTRUMENT_TO_SEARCH "ARCLK"
#define BUY_LOG_FILE_PATH "buy.txt"
#define BUY_LOG_FILE_PATH "sell.txt"

//FILE *sell_ptr, *buy_ptr;

/*CONTAINER FOR ORDERBOOK TABLE*/
//#define USE_RBT
#define USE_HEAP
//#define USE_LL
//#define USE_HASH

/*LOG*/ 
#define LOG_PRICE_CHANGES

/*DEBUG*/ 
//#define DEBUG_DELETE
//#define DEBUG_EXECUTE
//#define DEBUG_ADD

#define INPUT_ITCH_DIRECTORY "itch.pcap"
#define INSTRUMENT_TO_SEARCH "ARCLK"
#define SELL_LOG_FILE_PATH "sell_log.txt"
#define BUY_LOG_FILE_PATH "buy_log.txt"

#ifdef USE_HEAP
#include "mheap_orderbook.h"
#endif

#ifdef USE_RBT
#include "rbt_orderbook.h"
#endif 

#ifdef USE_LL
#include "ll_orderbook.h"
#endif 

#ifdef USE_HASH
#include "hash_orderbook.h"
#endif 

#define MOLDUDP64_HEARTBEAT_MESSAGE 0

#define ADD_ORDER       'A'
#define EXECUTE_ORDER   'E'
#define DELETE_ORDER    'D'
#define ADD_ORDER_MPID  'F'
#define ORDER_DIRECTORY 'R'

#define BUY  'B'
#define SELL 'S'

struct greater{
    bool operator()(std::pair<int, int> a, std::pair<int, int> b) const
    {
        return a.first>b.first;
    }
}

struct less{
    bool operator()(std::pair<int, int> a, std::pair<int, int> b) const
    {
        return a.first<b.first;
    }
}

#endif