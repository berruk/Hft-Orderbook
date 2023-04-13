/* 
    ITCH pcap parsing
    Order Book Table 

    ✭┈┈┈┈B┈E┈R┈R┈Y┈┈┈✭

    to do:

    [] benchmark google
    [] memory leak
    [] sınır price for hash

    [o] max and min price 
    [o] hash
    [o] heap : first search then add
    [o] open_pcap with nanosec something
    [o] log to file
    [o] order book id hash table
    [] rbt get_head_price. pointer?
    [o] log when change in buy side
    [o] config hisse için
        [o] get orderbook id from message with symbol
        [o] check orderbook id
    [o] time (nanosecond precision?)
    [] makefile 3 structure için
    [o] hash collision handling
    [x] execute test 
    [x] delete with given price fonksiyonu yaz
    [x] delete test
    [X] delete from all orders? equal to 0
    [x] max heap dene //ordered değil
    [x] min heap
    [x] linked list
    [x] integration linked list
    [x] all 3 options in orderbook.h
    [x] orderbook id specified 

    to answer:

    [] pricelar artarak azalarak geliyo mu??
    [] rank nasıl artıyor

    // gcc -ggdb -O0 parser.c orderbook.c linkedlist.c rbt.c maxminheap.c ip.c -o parser -lpcap

*/

#include <pcap.h>
#include <stdbool.h>
#include <stdint.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <net/ethernet.h>

#include "ip.hpp"
#include "itch.hpp"
#include "hash.h"
#include "orderbook.hpp"


uint64_t del_count = 0, ex_count = 0, add_count = 0;

/*
    Returns the price of an order with given order id and side
    Order id is unique per side
*/
int get_price_of_order_id(uint64_t order_id, uint8_t side)
{
    int hashed_order_id = hash(order_id);
    int index = hashed_order_id;


    while(order_id != order_book.orders[index][side].order_id)
    {
        index++;
    }

    int32_t price = order_book.orders[index][side].price;
    return price;
}

/*
    Returns the quantity of an order with given order id and side
    Order id is unique per side
*/
int get_quantity_of_order_id(uint64_t order_id, uint8_t side)
{
    int hashed_order_id = hash(order_id);
    int index = hashed_order_id;

    while(order_id != order_book.orders[index][side].order_id)
    {
        index++;
    }

    uint64_t quantity = order_book.orders[index][side].quantity;
    return quantity;
}

/*
    Returns remaining quantity of an order with order id
    after execution. 
    Zero means total execution and order is deleted.
*/
uint64_t decrease_quantity_of_order_id(uint64_t order_id, uint8_t side, uint64_t executed_quantity)
{
    int hashed_order_id = hash(order_id);
    int index = hashed_order_id;

    while(order_id != order_book.orders[index][side].order_id)
    {
        index++;
    }
    
    order_book.orders[index][side].quantity -= executed_quantity;

    if(order_book.orders[index][side].quantity == 0)
        order_book.orders[index][side].filled = 0;

    return order_book.orders[index][side].quantity;
}

/*
    Returns if a given order id is present in order book
    Order id is unique per side
*/
bool order_is_in_orderbook(uint64_t order_id, uint8_t side)
{

    int hashed_order_id = hash(order_id);
    int index = hashed_order_id;

    while(order_book.orders[index][side].order_id !=0)
    {
        if(order_book.orders[index][side].order_id == order_id)
            break;
        else 
            index++;
    }

    if(order_book.orders[index][side].order_id == order_id)
    {
        //printf("Returned price: %d\n", order_book.orders[index][side].price);
        //printf("Current quantity: %lu\n", order_book.orders[index][side].quantity);
        return true;
    }
    return false;
}

/*
    Executes the order given with order id and side by decreasing the quantity
    from the price point of order.
*/
void execute_order_in_orderbook(uint64_t order_id, uint8_t side, uint64_t quantity)
{
    
    int32_t price = get_price_of_order_id(order_id, side);
    decrease_quantity_of_order_id(order_id, side, quantity);
    
    //printf("Remaining quantity %ld\n",remaining_quantity);
    //printf("Side:%c Price: %d, Quantity: %lu\n", side, price, quantity);

    if(side == BUY)
    {
#ifdef DEBUG_EXECUTE
        log("####Before #####");
        print_buy();
        getchar();
        decrease_quantity_of_price_buy_side(price, quantity);
        log("####After####");
        print_buy();
        getchar();
#else
        decrease_quantity_of_price_buy_side(price, quantity);
#endif
    }
    
    else if(side == SELL)
    {
#ifdef DEBUG_EXECUTE
        log("####Before#####");
        print_sell(); 
        getchar();   
        decrease_quantity_of_price_sell_side(price, quantity);
        log("####After####");
        print_sell();
        getchar();
#else
        decrease_quantity_of_price_sell_side(price, quantity);
#endif
    }
}

/*
    Adds an order to list of all orders with given parameters
*/
void add_to_all_orders(uint64_t order_id, int32_t price, uint64_t quantity, uint8_t side)
{
    int hashed_order_id = hash(order_id);
    int index = hashed_order_id;

    //printf("Inıt index: %d",hashed_order_id);

    while(order_book.orders[index][side].filled == 1) //init with memset
    {
        index++;
    }

    if(hashed_order_id != index)
    {
        //printf("Chosen index: %d",index);
    }

    order_book.orders[index][side].order_id = order_id;
    order_book.orders[index][side].price = price;
    order_book.orders[index][side].quantity = quantity;
    order_book.orders[index][side].filled = 1;
}

/*
    Parses add order message and calls insert functions for buy sell sides.
*/
void handle_addorder_message(const u_char *data)
{
    struct OrderAdd *message = (struct OrderAdd *) data;
    uint32_t message_order_book_id = be32toh(message->order_book_id);
    int32_t price = be32toh(message->price);
    uint64_t quantity = be64toh(message->quantity);
    uint64_t order_id = be64toh(message->order_id);

    if(message_order_book_id != order_book.order_book_id)
        return;

    add_count++;
    
#ifdef DEBUG_ADD
    log("NEW ADD ORDER\n");
    printf("Price: %d, Quantity: %lu\n", price, quantity);
#endif 

    if(message->side == BUY)
    {
#ifdef DEBUG_ADD
        log("####BUY SIDE Before add#####");
        print_buy();
        getchar();
        insert_to_buy_side(price, quantity);
        log("####After add####");
        print_buy();
        getchar();
#else
        insert_to_buy_side(price, quantity);
#endif
    }
    else if(message->side == SELL)
    {
        #ifdef DEBUG_ADD
        log("####SELL SIDE Before add#####");
        print_sell();
        getchar();
        insert_to_sell_side(price, quantity);
        log("####After add####");
        print_sell();
        getchar();
#else 
        insert_to_sell_side(price, quantity);
#endif
    }

    add_to_all_orders(order_id, price, quantity, message->side);
}


/*
    Parses execute order message and calls execute function.
*/
void handle_executeorder_message(const u_char *data)
{
    struct OrderExecuted *message = (struct OrderExecuted *) data;
    uint32_t message_order_book_id = be32toh(message->order_book_id);
    uint64_t quantity = be64toh(message->quantity);
    uint64_t order_id = be64toh(message->order_id);
    uint8_t side = message->side;

    if(message_order_book_id != order_book.order_book_id)
        return;

    ex_count++;

#ifdef DEBUG_EXECUTE
    log("NEW EXECUTE ORDER\n");
#endif

    if(order_is_in_orderbook(order_id, side))
    {
        //printf("Executing quantity: %lu\n", quantity);
        execute_order_in_orderbook(order_id, side, quantity);
    }

}

/*
    Deletes order with given order id from list of all current orders
*/
void delete_from_all_orders(uint64_t order_id, uint8_t side)
{
    int hashed_order_id = hash(order_id);
    int index = hashed_order_id;

    while(order_book.orders[index][side].order_id != order_id)
    {
        index++;
    }
    
    //printf("Deleting from all orders price: %d quantity %lu\n", order_book.orders[index][side].price, order_book.orders[index][side].quantity);
    order_book.orders[index][side].order_id = 0;
    order_book.orders[index][side].filled = 0;
}

/*
    Deletes an order from sell side of order book by decreasing the quantity of the price point.
*/
void delete_order_from_sell_side(uint64_t order_id)
{    
    int32_t price = get_price_of_order_id(order_id, SELL);
    uint64_t quantity = get_quantity_of_order_id(order_id, SELL);
    
    //printf("DELETING from sell side Price: %d, Quantity: %lu", price, quantity);
    decrease_quantity_of_price_sell_side(price, quantity);

}

/*
    Deletes an order from buy side of order book by decreasing the quantity of the price point.
*/
void delete_order_from_buy_side(uint64_t order_id)
{
    
    int32_t price = get_price_of_order_id(order_id, BUY);
    uint64_t quantity = get_quantity_of_order_id(order_id, BUY);
    
    //printf("DELETING from buy side Price: %d, Quantity: %lu", price, quantity);
    decrease_quantity_of_price_buy_side(price, quantity);
    
}

/*
    Parses order book directory message and finds order book id of given instrument
*/
void handle_orderbook_directory(const u_char *data, char *given_symbol)
{
    struct OrderBookDirectory *message = (struct OrderBookDirectory *) data;
    char symbol[6];
    strncpy(symbol, message->symbol, 5);
    symbol[5] = '\0';

    //printf("SYMB: %s",message->symbol);
    //getchar();
    if(strcmp(symbol, given_symbol) == 0)
    {   
        order_book.order_book_id = be32toh(message->order_book_id);
        //printf("%s order book with id %u", symbol, order_book.order_book_id);
    }
}

/*
    Parses delete order message and calls delete function for buy and sell sides.
*/
void handle_deleteorder_message(const u_char *data)
{
    struct OrderDelete *message = (struct OrderDelete *) data;
    uint32_t message_order_book_id = be32toh(message->order_book_id);
    uint64_t order_id = be64toh(message->order_id);
    uint8_t side = message->side;

    if(message_order_book_id != order_book.order_book_id)
        return;

    del_count++;
 

    if(order_is_in_orderbook(order_id, side))
    {
#ifdef DEBUG_DELETE
        log("NEW DELETE ORDER\n");
        int32_t price = get_price_of_order_id(order_id, side);
        uint64_t quantity = get_quantity_of_order_id(order_id, side);
        printf("Price: %d, Quantity %lu", price, quantity);
#endif
        if(side == BUY)
        {
#ifdef DEBUG_DELETE
            log("####Before delete#####");
            print_buy();
            getchar();     
            delete_order_from_buy_side(order_id);
            log("####After####");
            print_buy();
            getchar(); 
#else
            delete_order_from_buy_side(order_id);
#endif
        }
        
        else if(side == SELL){
            
#ifdef DEBUG_DELETE
            log("####Before delete#####");
            print_sell();        
            getchar();                  
            delete_order_from_sell_side(order_id);
            log("####After####");
            print_sell();
            getchar();
#else
            delete_order_from_sell_side(order_id);
#endif
        }
        delete_from_all_orders(order_id, message->side);
    }
}

/*
*   @param  packet_data content of packet
*   @return connection partition number (4 ip and port combinations)
*/
uint32_t strip_headers(const uint8_t *packet_data)
{
    const struct ether_header *ethernet_header = (struct ether_header *) packet_data;                 
    const struct ip *ip_header;
    const struct udphdr *udp_header;

    //Find ethernet and ip headers
    if(be16toh(ethernet_header->ether_type) == ETHERTYPE_IP)                                          
    {
        ip_header = (struct ip *)(packet_data + sizeof(struct ether_header));
        
        if(ip_header->ip_p == IPPROTO_UDP)                                                            
        {
            udp_header = (struct udphdr*)
            (packet_data + sizeof(struct ether_header) + sizeof(struct iphdr));

            //Connection port and IP (VIOP0,1 BIST0,1)
            for(uint32_t partition_number = 0; partition_number < CONNECTION_TOTAL_NUMBER; partition_number++) 
            {
                if(be32toh(ip_header->ip_dst.s_addr) == itch_connection[partition_number].IP 
                && be16toh(udp_header->dest) == itch_connection[partition_number].port) 
                {
                    return partition_number;
                }
            }
        }
    }

    return -1;
}

/*
    Driver for ITCH message packets. 
    Calls appropriate function for message type
    log price changes
*/
void handle_messages(uint16_t message_count, uint8_t* data, char *instrument_to_search, struct pcap_pkthdr *packet_header)
{
    struct MessageBlock *message_block;
    uint32_t data_offset = 0;
    uint8_t message_type;

    for(uint16_t message = 0; message < message_count; message++)
    {
        message_block = (struct MessageBlock *)(data + data_offset);
        message_type = message_block->data[0];

        switch (message_type) 
        {
            case ADD_ORDER:
                //log("Add Order");
                handle_addorder_message(message_block->data);
                break;

            case EXECUTE_ORDER: 
                //log("Execute Order");
                handle_executeorder_message(message_block->data);
                break;

            case DELETE_ORDER:
                //log("Delete Order");
                handle_deleteorder_message(message_block->data);
                break;

            case ORDER_DIRECTORY:
                //log("Order Directory");
                handle_orderbook_directory(message_block->data, instrument_to_search);
                break;

            default:
                //log("None");
                break;
        }

#ifdef LOG_PRICE_CHANGES
        log_price_changes(&packet_header->ts);
#endif
        data_offset += sizeof(message_block->lenght) + be16toh(message_block->lenght);
    }
}


int main(void)
{
    /******** open pcap file ********/

    char errbuff[PCAP_ERRBUF_SIZE];    
    pcap_t* pcap = pcap_open_offline_with_tstamp_precision(INPUT_ITCH_DIRECTORY, PCAP_TSTAMP_PRECISION_NANO, errbuff); //nanosecond precision
    
	if(!pcap)
    {
		perror("Can't open file");
        exit(1);
	}

    log("Input file:"); log(INPUT_ITCH_DIRECTORY);
    
    char instrument_to_search[6] = INSTRUMENT_TO_SEARCH;
    
	struct pcap_pkthdr *packet_header;
	const uint8_t *packet_data;
    uint32_t partition_number;
    uint16_t message_count;
    uint32_t payload_index = sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr);
    DownstreamHeader *downstream_header;

    initialize_order_book();
    //sell_ptr = fopen(SELL_LOG_FILE_PATH, "a");
    //buy_ptr = fopen(BUY_LOG_FILE_PATH, "a");

    printf("Instrument: %s\n", instrument_to_search);
    //fflush(stdout);
    clock_t begin = clock();

    /******** read packets ********/
	while(pcap_next_ex(pcap, &packet_header, &packet_data) != PCAP_ERROR_BREAK)         
    {    
        if( (partition_number = strip_headers(packet_data)) != -1)
        {   
            downstream_header = (DownstreamHeader *)&packet_data[payload_index];
            message_count = be16toh(downstream_header->message_count);

            if(message_count == MOLDUDP64_HEARTBEAT_MESSAGE)
                continue;
            
            handle_messages(message_count, downstream_header->data, instrument_to_search, packet_header);
        }                   
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    
    printf("Total time elapsed: %f\n", time_spent);
    log("Message Counts:")
    printf("Add: %lu, Delete: %lu, Ex: %lu\n", add_count, del_count, ex_count);

    //fclose(buy_ptr);
    //fclose(sell_ptr);
}






