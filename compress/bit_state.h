#ifndef BIT_STATE_H
#define BIT_STATE_H

#include <stdint.h>

typedef struct BitMap
{
    uint64_t bits;
    uint8_t len;
} BitMap;

typedef struct BitState
{
    void *item;
    BitMap bits;
} BitState;

BitState *bs_create_bit_state(void *item);

#endif