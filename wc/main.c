#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    int option;
    int flag_c = 0;
    int flag_l = 0;
    int flag_w = 0;
    int flag_m = 0;
    int no_flags = 1;

    while ((option = getopt(argc, argv, "clwm")) != -1)
    {
        switch (option)
        {
        case 'c':
            flag_c = 1;
            no_flags = 0;
            break;
        case 'l':
            flag_l = 1;
            no_flags = 0;
            break;
        case 'w':
            flag_w = 1;
            no_flags = 0;
            break;
        case 'm':
            flag_m = 1;
            no_flags = 0;
            break;
        }
    }

    if (argc - optind > 1)
    {
        printf("You cannot provide more than 1 file to analyze.\n");
        exit(-1);
    }

    char *filepath;
    if (optind != argc)
    {
        filepath = argv[optind];
    }
    else
    {
        filepath = NULL;
    }

    FILE *file;
    if (!filepath)
    {
        file = stdin;
    }
    else
    {
        file = fopen(filepath, "rb");
    }

    if (!file)
    {
        printf("Could not find the specified file.\n");
        exit(-1);
    }

    int byte_count = 0;
    int line_count = 0;
    int word_count = 0;
    int char_count = 0;
    int in_word = 0;

    int c = fgetc(file);
    while (c != EOF)
    {
        byte_count++;
        char_count++;
        if (c == '\n')
        {
            line_count++;
        }
        if (!in_word)
        {
            if (!isspace(c))
            {
                // just found a new word
                in_word = 1;
            }
        }
        else
        {
            if (isspace(c))
            {
                // reached end of word
                in_word = 0;
                word_count++;
            }
        }
        c = fgetc(file);
    }

    if (file != stdin)
    {
        fclose(file);
    }

    printf("  ");
    if (flag_l || no_flags)
    {
        printf("%d ", line_count);
    }
    if (flag_w || no_flags)
    {
        printf("%d ", word_count);
    }
    if (flag_c || no_flags)
    {
        printf("%lu ", byte_count);
    }
    if (flag_m)
    {
        printf("%d ", char_count);
    }
    if (filepath != NULL)
    {
        printf("%s", filepath);
    }
}
