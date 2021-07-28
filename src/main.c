#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "fin/schema.h"
#include "fin/xml.h"

void print_help()
{
    printf("Usage: fin [OPTIONS]... [FILE]...\n");
    printf("\t-p, --print-accounts \tprint the accounts in file\n");


    printf("\t-h, --help \tshow this help message\n\n");
    printf("Author: Andre Brasil\n");
    printf("Report bugs to <brasil.a@protonmail.com>\n");
}

int main(int argc, char **argv)
{
    int opt;
    char *path_to_file = NULL;

    while((opt = getopt(argc, argv, "f:ph")) != -1)
    {
        switch(opt)
        {
            case 'f':
                path_to_file = optarg;
                break;
            case 'p':
                print_account_names(path_to_file);
                break;
            case 'h':
                print_help();
                break;
            case '?':
                //printf("Unknown option: %c\n", optopt);
                printf("Use -h to print the help page.\n");
                break;
        }
    }

    return 0;
}
