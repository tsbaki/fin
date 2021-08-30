#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

void
clear_screen()
{
    // TODO: Add windows compatibilty
    system("clear");
}

void 
print_welcome_msg()
{
    //TODO:
    printf("Welcome to fin 0.0.1!\n");
    printf("Type 'help' to list commands...\n");
}

void 
print_help()
{
    printf("\nVersion 0.0.1\n");
    printf("Commands:\n");
    printf("\t- list [la/lt] [accounts/transactions]    List the accounts in this db.\n");
    printf("\t- add [accounttransaction]                Add a transaction/account\n");
    printf("\nAuthor: Andre Brasil\n");
    printf("Report bugs to <brasil.a@pm.me>\n\n");
}

void 
remove_trailing_chars(char *string)
{
    if(string == NULL)
        return;

    int last_char = strlen(string) - 1;

    if(string[last_char] == '\n')
        string[last_char] = '\0';
}

bool 
starts_with(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre);
    size_t lenstr = strlen(str);
    
    return lenstr < lenpre ? 
        false : memcmp(pre, str, lenpre) == 0;
}

bool
check_file_exists(char *path)
{
    if (access(path, F_OK) != -1)
        return true;
    else  
        return false;
}
