#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

int main() {
    FILE *sound_file = fopen("sound.wav", "wb"); 
    if (sound_file == NULL) {
        printf("Could not create sound file");
        return EXIT_FAILURE;
    }

    uint32_t frequency = 44100;
    uint32_t duration = 3;
    uint16_t channels = 2;
    uint16_t bits_per_sample = 16;
    uint16_t bytes_per_block = channels * bits_per_sample / 8;
    uint32_t bytes_per_sec = frequency * bytes_per_block;
    uint32_t data_size = bytes_per_sec * duration;
    uint32_t file_size = data_size + 44 - 8; // add header and remove 8
    uint32_t chunk_size = 16;
    uint16_t data_format = 1; // pcm integer

    // master RIFF chunk
    fwrite("RIFF", 4, 1, sound_file);
    fwrite(&file_size, 4, 1, sound_file);
    fwrite("WAVE", 4, 1, sound_file);

    // format chunk
    fwrite("fmt ", 4, 1, sound_file);
    fwrite(&chunk_size, 4, 1, sound_file);
    fwrite(&data_format, 2, 1, sound_file);
    fwrite(&channels, 2, 1, sound_file);
    fwrite(&frequency, 4, 1, sound_file);
    fwrite(&bytes_per_sec, 4, 1, sound_file);
    fwrite(&bytes_per_block, 2, 1, sound_file);
    fwrite(&bits_per_sample, 2, 1, sound_file);

    // sampled data
    fwrite("data", 4, 1, sound_file);
    fwrite(&data_size, 4, 1, sound_file);
    uint32_t frames = frequency*duration;
    for (uint32_t i = 0; i < frames/2; i++) {
        float t = (float)i / frequency;
        float y = 1.0f * sinf(2.0f * 3.14159265f * 440.0f * t);
        int16_t pcm_integer_y = (int16_t)(INT16_MAX * y);
        // printf("%i\n", pcm_integer_y);
        fwrite(&pcm_integer_y, sizeof(pcm_integer_y), 1, sound_file); // right channel
        fwrite(&pcm_integer_y, sizeof(pcm_integer_y), 1, sound_file); // left channel
    }

    for (uint32_t i = 0; i < frames/2; i++) {
        float t = (float)i / frequency;
        float y = 1.0f * sinf(2.0f * 3.14159265f * 261.0f * t);
        int16_t pcm_integer_y = (int16_t)(INT16_MAX * y);
        // printf("%i\n", pcm_integer_y);
        fwrite(&pcm_integer_y, sizeof(pcm_integer_y), 1, sound_file); // right channel
        fwrite(&pcm_integer_y, sizeof(pcm_integer_y), 1, sound_file); // left channel
    }



    return EXIT_SUCCESS;
}
