#ifndef IP_CONNECTION
#define IP_CONNECTION

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <stdint.h>

#define MAX_INSTRUMENT_NUM 200

enum destination_ip {
    connection_0,
    connection_1,
    connection_2,
    connection_3,
    CONNECTION_TOTAL_NUMBER
};

struct connection {
    uint32_t IP;
    uint32_t port;
    char *name;
};

struct order_book_id_struct {
    uint32_t partition_id;
    char name[20];
    uint32_t num;
};

extern struct connection itch_connection[CONNECTION_TOTAL_NUMBER];

#endif 