#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

static const uint32_t FREQUENCY_HZ = 44100;
static const uint32_t DURATION_S = 3;
static const uint16_t CHANNELS = 2;

void write_header(FILE *file) {
    uint16_t bits_per_sample = 16;
    uint16_t bytes_per_block = CHANNELS * bits_per_sample / 8;
    uint32_t bytes_per_sec = FREQUENCY_HZ * bytes_per_block;
    uint32_t data_size = bytes_per_sec * DURATION_S;
    uint32_t file_size = data_size + 44 - 8; // add header and remove 8
    uint32_t chunk_size = 16;
    uint16_t data_format = 1; // pcm integer

    // master RIFF chunk
    fwrite("RIFF", 4, 1, file);
    fwrite(&file_size, sizeof(file_size), 1, file);
    fwrite("WAVE", 4, 1, file);

    // format chunk
    fwrite("fmt ", 4, 1, file);
    fwrite(&chunk_size, sizeof(chunk_size), 1, file);
    fwrite(&data_format, sizeof(data_format), 1, file);
    fwrite(&CHANNELS, sizeof(CHANNELS), 1, file);
    fwrite(&FREQUENCY_HZ, sizeof(FREQUENCY_HZ), 1, file);
    fwrite(&bytes_per_sec, sizeof(bytes_per_sec), 1, file);
    fwrite(&bytes_per_block, sizeof(bytes_per_block), 1, file);
    fwrite(&bits_per_sample, sizeof(bits_per_sample), 1, file);

    // data chunk
    fwrite("data", 4, 1, file);
    fwrite(&data_size, sizeof(data_size), 1, file);
}

int main() {
    FILE *sound_file = fopen("sound.wav", "wb"); 
    if (sound_file == NULL) {
        printf("Could not create sound file");
        return EXIT_FAILURE;
    }

    write_header(sound_file);

    // write sampled data
    uint32_t frames = FREQUENCY_HZ*DURATION_S;
    for (uint32_t i = 0; i < frames; i++) {
        float t = (float)i / FREQUENCY_HZ;
        float y = 1.0f * sinf(2.0f * 3.14159265f * 440.0f * t);
        int16_t pcm_integer_y = (int16_t)(INT16_MAX * y);
        // printf("%i\n", pcm_integer_y);
        fwrite(&pcm_integer_y, sizeof(pcm_integer_y), 1, sound_file); // right channel
        fwrite(&pcm_integer_y, sizeof(pcm_integer_y), 1, sound_file); // left channel
    }

    return EXIT_SUCCESS;
}
