#ifndef BIT_WRITER_H
#define BIT_WRITER_H

#include <stdio.h>
#include <stdint.h>
#include "bit_state.h"

typedef struct BitWriter
{
    FILE *file;
    uint8_t buffer;
    uint8_t len;
    uint64_t written_bytes;
} BitWriter;

BitWriter *bw_create_bit_writer(FILE *file);

void bw_write_bits(BitWriter *bit_writer, BitMap bit_map);

void bw_flush_buffer(BitWriter *bit_writer);

#endif