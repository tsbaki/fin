#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "schema.h"
#include "utils.h"
#include "db.h"

void init_account(char *acc_name)
{
    
    // Will hold the command
    char co[100];
    
    // TODO: Should short the path to file if too long
    // TODO: Include balance of the account in parenthesis.
    printf("fin: %s > ", acc_name); 

    fgets(co, 100, stdin);
    
    // This should be add accoun
    if(strcmp(co, "add expense\n") == 0)
    {
        expense_t exp;
        time_t rawtime;
        struct tm * timeinfo;
        
        time(&rawtime);

        timeinfo = localtime(&rawtime);

        strcpy(exp.from, acc_name);

        printf("Creating expense\n");

        printf("> Amount: ");
        scanf("%lf", &exp.amount);

        printf("> Reference (default: <empty>): ");
        scanf("%s", exp.ref);

        printf("> To: ");
        scanf("%s", exp.to);

        char buffer[30];

        sprintf(buffer, "%s:%s:%s %s/%s/%s",
                timeinfo->tm_hour,
                timeinfo->tm_min,
                timeinfo->tm_sec,
                timeinfo->tm_yday,
                timeinfo->tm_mon,
                timeinfo->tm_year);


        strcpy(exp.timestamp, buffer);

        printf("Expense from '%s' has been created\n", acc_name);

    }
    else if(strcmp(co, "help\n") == 0)
    {
        print_help();
    }
    else if(strcmp(co, "clear\n") == 0)
    {
        clear_screen();
    }
    else if(strcmp(co, "main\n") == 0)
    {
        return;
    }
    else 
    {
        printf("Unknown options: %s\n", co);
        printf("Type 'help' to list commands\n", co);
    }

    init_account(acc_name);

}
