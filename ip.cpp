#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <stdint.h>
#include "ip.hpp"

struct connection itch_connection[CONNECTION_TOTAL_NUMBER] = {

    { .IP = 3916552243, .port = 21001, .name = "itch_bist_0"},  
    { .IP = 3916552243, .port = 21002, .name = "itch_bist_1"},
    { .IP = 3916552245, .port = 21001, .name = "itch_viop_0"},
    { .IP = 3916552245, .port = 21002, .name = "itch_viop_1"},

};