#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "import.h"
#include "export.h"

int main(int argc, char *argv[])
{
    bool isCaseInsensitive = false;

    int opt;

    enum { CHARACTER_MODE, WORD_MODE, LINE_MODE } mode = CHARACTER_MODE;

    // Read this to understand how this is implemented.
    //https://www.tutorialspoint.com/getopt-function-in-c-to-parse-command-line-arguments
    while((opt = getopt(argc, argv, "ilw")) != -1) {
        switch (opt) {
        case 'i': 
            isCaseInsensitive = true; 
            break;
        case 'l': 
            mode = LINE_MODE; 
            break;
        case 'w': 
            mode = WORD_MODE; 
            break;
        default:
            fprintf(stderr, "Usage: %s [-ilw] [file...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Now optind (declared extern int by <unistd.h>) is the index of the first non-option argument.
    // If it is >= argc, there were no non-option arguments.

    // ...
    return 0;
}
