#include <stdint.h>
#define SIZE_HASHMAP 80000

//linear probing

static inline uint16_t hash(uint64_t input)
{
    uint16_t output = input % SIZE_HASHMAP + (input/10000) % 100;
    //printf("Hashed: %u", output);
    return output;

}