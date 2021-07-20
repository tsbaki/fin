#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "import/import.h"
#include "export/export.h"

int main(int argc, char *argv[])
{
    int opt;

    /* Options:
     *  File, the path to the file being used.
     *  Commands:
     *      -p Print.
     *      -c Make transaction 
    */
    while((opt = getopt(argc, argv, "f:")) != -1) {

        switch (opt) {
        case 'f':
            printf("%s\n", optarg);
        break;
        default:
            fprintf(stderr, "Usage: %s [-ilw] [file...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }

    }

    // Now optind (declared extern int by <unistd.h>) is the index of the first non-option argument.
    // If it is >= argc, there were no non-option arguments.
    printf("The file %s", argv[optind]);

    return 0;
}
