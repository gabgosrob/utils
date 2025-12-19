#include <stdlib.h>
#include "bit_writer.h"

BitWriter *bw_create_bit_writer(FILE *file)
{
    BitWriter *bit_writer = malloc(sizeof(BitWriter));
    if (!bit_writer)
    {
        return NULL;
    }

    bit_writer->file = file;
    bit_writer->buffer = 0;
    bit_writer->len = 0;

    return bit_writer;
}

void bw_write_bits(BitWriter *bit_writer, BitMap bit_map)
{
    // iterate in reverse to extract from MSB to LSB
    for (int i = bit_map.len - 1; i >= 0; i--)
    {
        // put bit in LSB and set all others to 0
        uint8_t bit = (bit_map.bits >> i) & 1;

        // add bit to buffer
        bit_writer->buffer = (bit_writer->buffer << 1) | bit;
        bit_writer->len++;

        // write and flush buffer when full
        if (bit_writer->len == 8)
        {
            fwrite(&bit_writer->buffer, sizeof(bit_writer->buffer), 1, bit_writer->file);
            bit_writer->buffer = 0;
            bit_writer->len = 0;
        }
    }
}

void bw_flush_buffer(BitWriter *bit_writer)
{
    if (bit_writer->len != 0)
    {
        uint8_t missing_bits = 8 - bit_writer->len;
        // add missing bits to buffer
        bit_writer->buffer = bit_writer->buffer << missing_bits;
        fwrite(&bit_writer->buffer, sizeof(bit_writer->buffer), 1, bit_writer->file);
        bit_writer->buffer = 0;
        bit_writer->len = 0;
    }
}