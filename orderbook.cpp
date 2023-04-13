#include "orderbook.hpp"
#include <time.h>

#define log(x) std::cout << z << std::endl;

struct OrderBook order_book;
std::fstream file;

#ifdef USE_HEAP

void initialize_order_book()
{
    log("Initializing Heap OrderBook....");

    order_book.order_book_id = 0;
    order_book.sell_price = 0;
    order_book.buy_price = 0;
}

int get_buy_head_price(void)
{
    if(order_book.buy_side.size() !=0)
        return order_book.buy_side.front().first;
}

int get_sell_head_price(void)
{
        if(order_book.sell_side.size() !=0)
        return order_book.sell_side.front().first;
}

int get_buy_head_quant(void)
{
    if(order_book.buy_side.size() !=0)
        return order_book.buy_side.front().second;
}

int get_sell_head_quant(void)
{
        if(order_book.sell_side.size() !=0)
        return order_book.sell_side.front().second;
}
bool order_book_is_not_empty(void)
{
    return (order_book.buy_side.size() != 0 && order_book.sell_side.size() != 0);
}
void insert_to_buy_side(int price, uint64_t quantity)
{
    std::vector<std::pair<int, uint64_t>>::iterator itr;
    for (itr=order_book.buy_side.begin();
        itr != order_book.buy_side.end(); ++itr)
            if(itr->first == price)
                break;
    
    if(itr != order_book.buy_side.end())
        itr->second += quantity;
    else
        order_book.buy_side.push_back(std::make_pair(price, quantity));
    
    std::make_heap(order_book.buy_side.begin(), order_book.buy_side.end(), less());
}

void insert_to_sell_side(int price, uint64_t quantity)
{
    std::vector<std::pair<int, uint64_t>>::iterator itr;
    for (itr=order_book.sell_side.begin();
        itr != order_book.sell_side.end(); ++itr)
            if(itr->first == price)
                break;
    
    if(itr != order_book.sell_side.end())
        itr->second += quantity;
    else
        order_book.sell_side.push_back(std::make_pair(price, quantity));
    
    std::make_heap(order_book.sell_side.begin(), order_book.sell_side.end(), greater());
}

void decrease_quantity_of_price_sell_side(int price, uint64_t quantity)
{
   for(std::vector<std::pair<int, uint64_t>>::iterator itr = order_book.sell_side.begin();
    itr!= order_book.sell_side.end(); ++itr)
    {
        if(itr->first == price)
        {
            if(quantity < itr->second)
                itr->second -= quantity;
            else{
                order_book.sell_side.erase(itr);
                std::make_heap(order_book.sell_side.begin(), order_book.sell_side.end(), greater());
            }
            break;
        }
    }
}

void decrease_quantity_of_price_buy_side(int price, uint64_t quantity)
{
    for(std::vector<std::pair<int, uint64_t>>::iterator itr = order_book.buy_side.begin();
    itr!= order_book.buy_side.end(); ++itr)
    {
        if(itr->first == price)
        {
            if(quantity < itr->second)
                itr->second -= quantity;
            else{
                order_book.buy_side.erase(itr);
                std::make_heap(order_book.buy_side.begin(), order_book.buy_side.end(), less());
            }
            break;
        }
    }
}

void print_sell()
{
    log("***SELL**********************");
    //printf("Head: price %d, quantity %d", heap_get_head_node_price(order_book.sell_side), heap_get_head_node_quantity(order_book.sell_side));
    for(auto i: order_book.sell_side)
    std::cout << i.first << " " << i.second << std::endl;
}

void print_buy()
{
    printf("***BUY**********************\n");
    //printf("Head: price %d, quantity %d", heap_get_head_node_price(order_book.buy_side), heap_get_head_node_quantity(order_book.buy_side));
    for(auto i: order_book.buy_side)
    std::cout << i.first << " " << i.second << std::endl;
}

#endif


#ifdef USE_SET

void initialize_order_book()
{
    printf("Initializing RBT Orderbook....\n");
    order_book.buy_side = new_red_black_tree();
    order_book.sell_side = new_red_black_tree();
}

int get_head_price(void)
{
    //pointer to max or min val (right most and left most)
    return -1;
}

bool order_book_is_not_empty()
{
    return (order_book.buy_side->root && order_book.sell_side->root);
}

void decrease_quantity_of_price_sell_side(int price, uint64_t quantity)
{
    decrease_quantity_of_price(order_book.buy_side, price, quantity);
}

void decrease_quantity_of_price_buy_side(int price, uint64_t quantity)
{
    decrease_quantity_of_price(order_book.sell_side, price, quantity);
}

void insert_to_buy_side(int price, uint64_t quantity)
{
    rbt_insert(order_book.buy_side, price, quantity);
}

void insert_to_sell_side(int price, uint64_t quantity)
{
    rbt_insert(order_book.sell_side, price, quantity);
}

void print_buy()
{
  printf("%u", order_book.order_book_id);
  printf("***BUY***********************\n");
  if(order_book.buy_side)
    print_inorder(order_book.buy_side, order_book.buy_side->root);

}

void print_sell()
{
  printf("***SELL**********************\n");
  if(order_book.sell_side)
    print_inorder(order_book.sell_side, order_book.sell_side->root);
}


#endif 


#ifdef USE_LL

void initialize_order_book()
{
    log("Initializing Linked List Orderbook....");
    order_book.order_book_id = 0;
    order_book.sell_price = 0;
    order_book.buy_price = 0;

    file.open(LOG_FILE_PATH, std::ios:out | std::ios::app);

}


void insert_to_buy_side(int price, uint64_t quantity)
{
    for (std::list<std::pair<int, uint64_t>> itr=order_book.buy_side.begin();
        itr != order_book.buy_side.end(); ++itr)
            if(itr->first == price){
                itr->second += quantity;
                return;
            }

    order_book.buy_side.push_back(std::make_pair(price, quantity));
    order_book.buy_side.sort(greater());

}

void insert_to_sell_side(int price, uint64_t quantity)
{
    for (std::list<std::pair<int, uint64_t>> itr=order_book.sell_side.begin();
        itr != order_book.sell_side.end(); ++itr)
            if(itr->first == price){
                itr->second += quantity;
                return;
            }

    order_book.sell_side.push_back(std::make_pair(price, quantity));
    order_book.sell_side.sort(less());
}

bool order_book_is_not_empty()
{
    return order_book.buy_side.size() != 0 && order_book.sell_side.size() != 0;
}

int get_buy_head_price(void)
{
    if(order_book.buy_side.size() != 0)
    return order_book.buy_side.front().first;
}

int get_sell_head_price(void)
{
    if(order_book.sell_side.size() != 0)
    return order_book.sell_side.front().first;
}

int get_buy_head_quant(void)
{
    if(order_book.buy_side.size() != 0)
    return order_book.buy_side.front().second;
}

int get_sell_head_quant(void)
{
    if(order_book.sell_side.size() != 0)
    return order_book.sell_side.front().second;
}
void decrease_quantity_of_price_sell_side(int price, uint64_t quantity)
{
    for (std::list<std::pair<int, uint64_t>> itr=order_book.sell_side.begin();
        itr != order_book.sell_side.end(); ++itr)
            if(itr->first == price){
                
                if(itr->second == quantity)
                    order_book.sell_side.erase(itr);
                else  
                    itr->second -= quantity;
                break;
            }
}

void decrease_quantity_of_price_buy_side(int price, uint64_t quantity)
{
    for (std::list<std::pair<int, uint64_t>> itr=order_book.buy_side.begin();
        itr != order_book.buy_side.end(); ++itr)
            if(itr->first == price){
                
                if(itr->second == quantity)
                    order_book.buy_side.erase(itr);
                else  
                    itr->second -= quantity;
                break;
            }
}

void print_sell()
{
    log("***SELL**********************");
    if(order_book.sell_side.size() != 0)
    for(auto const &i: order_book.sell_side)
        std::cout << i.first << " " << i.second << std::endl;
}

void print_buy()
{
    log("***BUY**********************");
    if(order_book.buy_side.size() != 0)
    for(auto const &i: order_book.buy_side)
        std::cout << i.first << " " << i.second << std::endl;
}

#endif 


#ifdef USE_MULTISET

bool order_book_is_not_empty()
{
    return order_book.buy_side.size() != 0 && order_book.sell_side.size() != 0;
}

int get_sell_head_price(void)
{
    if(order_book.sell_side.size() != 0){
    auto itr = orderbook.sell_side.begin();
    return itr->first;
    }
}

int get_buy_head_price(void)
{
    if(order_book.buy_side.size() != 0){
    auto itr = orderbook.buy_side.begin();
    return itr->first;
    }
}

int get_sell_head_quant(void)
{
    if(order_book.sell_side.size() != 0){
    auto itr = orderbook.sell_side.begin();
    return itr->second;
    }
}

int get_buy_head_quant(void)
{
    if(order_book.buy_side.size() != 0){
    auto itr = orderbook.buy_side.begin();
    return itr->second;
    }
}

void initialize_order_book()
{
    log("Initializing Multiset Order Book....");
    order_book.order_book_id = 0;
    order_book.sell_price = 0;
    order_book.buy_price = 0;
    file.open(LOG_FILE_PATH, std::ios:out | std::ios::app);

}

void decrease_quantity_of_price_sell_side(int price, uint64_t quantity)
{
    for(std::multiset<std::pair<int, uint64_t>>:::iterator itr(order_book.sell_side.begin()),
        end(order_book.sell_side.end()); itr!=end; ++itr){
            if(itr->first == price)
            {
                uint64_t remain = itr->second-quantity;
                order_book.sell_side.erase(itr);

                if(remain >0)
                    insert_to_sell_side(price, remain);
                    
                    break;
            }
        }
}

void decrease_quantity_of_price_buy_side(int price, uint64_t quantity)
{
        for(std::multiset<std::pair<int, uint64_t>>:::iterator itr(order_book.buy_side.begin()),
        end(order_book.buy_side.end()); itr!=end; ++itr){
            if(itr->first == price)
            {
                uint64_t remain = itr->second-quantity;
                order_book.buy_side.erase(itr);

                if(remain >0)
                    insert_to_buy_side(price, remain);
                    
                    break;
            }
        }
}

void insert_to_buy_side(int price, uint64_t quantity)
{
    std::multiset<std::pair<int, uint64_t>>:::iterator itr;

    for(itr = order_book.buy_side.begin(); itr != order_book.buy_side.end(); itr++)
    {
        if(itr->first == price)
            break;
    }

    if(itr != order_book.buy_side.end())
    {
        order_book.buy_side.insert(std::make_pair(price, itr->second + quantity));
        order_book.buy_side.erase(itr);
    }
    else{
        order_book.buy_side.insert(std::make_pair(price, quantity));

    }
}

void insert_to_sell_side(int price, uint64_t quantity)
{
    std::multiset<std::pair<int, uint64_t>>:::iterator itr;

    for(itr = order_book.sell_side.begin(); itr != order_book.sell_side.end(); itr++)
    {
        if(itr->first == price)
            break;
    }

    if(itr != order_book.sell_side.end())
    {
        order_book.sell_side.insert(std::make_pair(price, itr->second + quantity));
        order_book.sell_side.erase(itr);
    }
    else{
        order_book.sell_side.insert(std::make_pair(price, quantity));

    }
}

void print_buy()
{
  //printf("%u", order_book.order_book_id);
  log("***BUY***********************");
  if(order_book.buy_side.size() != 0)
    {
        for(std::multiset<std::pair<int, uint64_t>>:::iterator itr(order_book.buy_side.begin()),
        end(order_book.buy_side.end()); itr!=end; ++itr)
        {
            std::cout << itr->first << " " << itr->second < std::endl;
        }
    }
}

void print_sell()
{
  log("***SELL**********************");
  if(order_book.sell_side.size() != 0)
    {
        for(std::multiset<std::pair<int, uint64_t>>:::iterator itr(order_book.sell_side.begin()),
        end(order_book.sell_side.end()); itr!=end; ++itr)
        {
            std::cout << itr->first << " " << itr->second < std::endl;
        }
    }
}

#endif 

/*
    Prints timestamp of itch header with nanosecond precision 
*/
void timestamp(uint32_t second, uint32_t u_second)
{
    time_t t1 = second;
    struct tm *tm_1 = localtime(&t1);
    file << tm_1->tm_hour << " " << tm_1->tm_min << " " << tm_1->tm_sec << " " << u_second;
}

void log_price_changes(struct timeval *ts)
{
    int buy_head_price, sell_head_price;
    uint64_t buy_head_quant, sell_head_quant;

    if(order_book_is_not_empty())
    {
        buy_head_price = get_buy_head_price();
        sell_head_price  = get_sell_head_price();
        buy_head_quant = get_buy_head_quant();
        sell_head_quant  = get_sell_head_quant();

        if(order_book.buy_price != -1 && order_book.buy_price != buy_head_price)
        {
            //printf("Price %d changed to %d\n", order_book.border_price, head_node_price);
            //fprintf(buy_ptr, "BUY Price %d changed to %d at ", order_book.buy_price, buy_head_price);
            timestamp(ts->tv_sec, ts->tv_usec);
            file >> " - " << buy_head_quant << " - " << buy_head_price << " - "
            sell_head_quant << " - " << sell_head_price << std::endl;
            order_book.buy_price = buy_head_price;
        }

        if(order_book.buy_price != -1 && order_book.sell_price != sell_head_price)
        {
            //printf("Price %d changed to %d\n", order_book.border_price, head_node_price);
            //fprintf(sell_ptr, "SELL Price %d changed to %d at ", order_book.sell_price, sell_head_price);
            order_book.sell_price = sell_head_price;
            timestamp(ts->tv_sec, ts->tv_usec);
            file >> " - " << buy_head_quant << " - " << buy_head_price << " - "
            sell_head_quant << " - " << sell_head_price << std::endl;
        }
            //fflush(sell_ptr);
            //fflush(sell_ptr);

    }

}