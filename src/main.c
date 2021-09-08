#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schema.h"
#include "db.h"
#include "utils.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char *db_path = "";

void init()
{
    /* must be called first */
    set_db(db_path);
    
    /* Command entered by the user */
    char co[20];

    printf("fin: %s > ", db_path);

    fgets(co, 20, stdin);

    if(co == NULL)
    {
        printf("Enter 'quit' to quit the program\n");
    }
    else if(strcmp(co, "l\n") == 0)
    {
        print_account_names();
    } 
    else if(strcmp(co, "da\n") == 0)
    {
        char account[20];

        print_account_names();

        printf("Account: ");
        fgets(account, 20, stdin);

        if(account_exists(account) == 1)
            delete_account(account);
        else 
            printf("Account does not exist.\n");
    }
    else if(strcmp(co, "lt\n") == 0)
    {

        char account[20];

        print_account_names();

        printf("Account: ");
        fgets(account, 20, stdin);

        if(account_exists(account) == 1)
        {
            size_t arr_size = 0;

            transaction_t *transactions = 
                load_transactions_for_account(account,
                        &arr_size);

            if(arr_size == 0) 
            {
                printf("No transactions to show yet\n");
            } else 
            {
                for(size_t i=0; i < arr_size; i++)
                {
                    if (i>0)
                    {
                        /* Don't print duplicated timestamps */
                        if (strcmp(transactions[i-1].timestamp,
                                    transactions[i].timestamp) != 0)
                            printf("[%s]\n",
                                   transactions[i].timestamp);
                    } else
                    {
                        printf("[%s]\n", transactions[i].timestamp);
                    }

                    printf("\t%s: ", transactions[i].ref);

                    if(transactions[i].amount < 0)
                        printf(ANSI_COLOR_RED "%lf\n"  
                                ANSI_COLOR_RESET, 
                                transactions[i].amount);
                    else
                        printf(ANSI_COLOR_GREEN "+%lf\n"  
                                ANSI_COLOR_RESET, 
                                transactions[i].amount);
                }
            }

        } else
        {
            printf("`%s` does not exist\n", account);
        }

    } else if(strcmp(co, "add account\n") == 0  || strcmp(co, "aa\n") == 0)
    {
        account_t acc;

        char name[20];
        char line[256];
        double balance;
        double goal;

        printf("Name: ");
        fgets(name, 20, stdin);

        printf("Balance (e.g 20.0): ");

        if (fgets(line, sizeof(line), stdin))
            if (1 != sscanf(line, "%lf", &balance))
                printf("There was an error processing the value.\n");

        printf("Goal (The amount you want to save to): ");
        scanf("%lf", &goal);

        remove_trailing_chars(name);

        strcpy(acc.name, name);

        acc.balance = balance;

        acc.goal = goal;

        insert_new_account(&acc);

    } else if(strcmp(co, "add transaction\n") == 0
            || strcmp(co, "at\n") == 0)
    {
        char ref[10];
        char account[50];
        double amount;

        print_account_names();

        printf("Account: " );
        scanf("%s", account);

        if(account_exists(account) == 1)
        {
            printf("Amount: ");
            scanf("%lf", &amount);

            if(amount == 0)
                printf("Invalid number.\n");

            printf("Reference (e.g Groceries): ");
            scanf("%s", ref);

            register_transaction(ref, account, amount);
        }
        else
        {
            printf("Account %s does not exist...\n",account);
        }

    } else if(strcmp(co, "help\n") == 0)
    {
        print_help();
    } else if(strcmp(co, "clear\n") == 0)
    {
        clear_screen();
    } else if(strcmp(co, "quit\n") == 0)
    {
        printf("Bye\n");
        return;
    } else 
    {
        if(strcmp(co, "\n") != 0) 
        {
            printf("\nUnknown options: %s\n", co);
            printf("Type 'help' to list commands\n\n", co);
        }
    }

    init();
}

int 
main(int argc, char **argv)
{

    if(argc == 1)
    {
        printf("ERROR: Provide path to file.\n");
        printf("Use -n <name> to start with new db.\n");
        return 1;
    }
    else if(strcmp(argv[1], "-n") == 0) 
    {
        start_empty_db(argv[2]);
        db_path = argv[2];
    }
    else if(argc > 2) 
    {
        printf("ERROR: Too many arguments.\n");
        return 1;
    }

    if(strcmp(db_path, "") == 0)
        db_path = argv[1];

    if(check_file_exists(db_path))
    {
        print_welcome_msg();
        init();
    }
    else 
    {
        printf("ERROR: '%s' does not exist.\n", db_path);
        return 1;
    }

    return 0;
}
