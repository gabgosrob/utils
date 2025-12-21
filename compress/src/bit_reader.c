#include <stdlib.h>
#include "bit_reader.h"

BitReader *br_create_bit_reader(FILE *file)
{
    BitReader *bit_reader = malloc(sizeof(BitReader));
    if (!bit_reader)
    {
        return NULL;
    }

    bit_reader->file = file;
    bit_reader->buffer = 0;
    bit_reader->pos = 0;

    fread(&bit_reader->buffer, sizeof(bit_reader->buffer), 1, bit_reader->file);

    return bit_reader;
}

uint8_t br_read_bit(BitReader *bit_reader)
{
    // read MSB at pos into buffer
    uint8_t bit = (bit_reader->buffer >> (7 - bit_reader->pos)) & 1;
    bit_reader->pos++;

    // put next byte into buffer when all bits read
    if (bit_reader->pos == 8)
    {
        fread(&bit_reader->buffer, sizeof(bit_reader->buffer), 1, bit_reader->file);
        bit_reader->pos = 0;
    }

    return bit;
}