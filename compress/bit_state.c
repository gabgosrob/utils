#include <stdlib.h>
#include "bit_state.h"

BitState *bs_create_bit_state(void *item)
{
    BitState *state = malloc(sizeof(BitState));
    if (!state)
    {
        return NULL;
    }

    state->item = item;
    state->bits.bits = 0;
    state->bits.len = 0;

    return state;
}