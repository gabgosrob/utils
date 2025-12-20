#include <stdio.h>
#include <unistd.h>
#include "compression.h"

int main(int argc, char *argv[])
{
    int option;
    int flag_c = 0;
    int flag_u = 0;
    int both_flags = 0;

    while ((option = getopt(argc, argv, "cu")) != -1)
    {
        switch (option)
        {
        case 'c':
            flag_c = 1;
            break;
        case 'u':
            flag_u = 1;
            break;
        }
    }
    both_flags = flag_c && flag_u;

    if (argc - optind != 1)
    {
        printf("You must provide exactly 1 file to compress or uncompress.\n");
        exit(-1);
    }
    if (!flag_c && !flag_u)
    {
        printf("You must specify whether to compress or uncompress.\n");
        exit(-1);
    }
    if (both_flags)
    {
        printf("Cannot compress and uncompress at the same time.\n");
        exit(-1);
    }

    if (flag_c)
    {
        compress(argv[optind]);
    }
    else if (flag_u)
    {
        uncompress(argv[optind]);
    }

    return 0;
}