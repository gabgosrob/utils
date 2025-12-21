#ifndef BIT_READER_H
#define BIT_READER_H

#include <stdio.h>
#include <stdint.h>

typedef struct BitReader
{
    FILE *file;
    uint8_t buffer;
    uint8_t pos;
} BitReader;

BitReader *br_create_bit_reader(FILE *file);

uint8_t br_read_bit(BitReader *bit_reader);

#endif