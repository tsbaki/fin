#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "fin/schema.h"
#include "fin/xml.h"

char acc_name[7] = "savings";

void
print_test_result(char *test_name, bool passed)
{
    printf("Test: %s : [%s]\n", 
            test_name, 
            passed == 1 ? "PASS" : "FAIL");
}

void 
test_create_account()
{
    account_t tmp_account;

    strcpy(tmp_account.name, acc_name);
    tmp_account.balance = 200.0;

    save_account(&tmp_account);

}

void 
test_load_account()
{
    account_t usr_account;
    usr_account = load_account(acc_name);

    print_test_result("test_create_load_account", 
           strcmp(usr_account.name, acc_name) == 0);
}

void
test_register_transaction()
{
    // Create transaction and add it to the account
    transaction_t trans;
    strcpy(trans.from, "day_to_day");
    strcpy(trans.to, "landlord");
    trans.amount = 100.0;
}

void 
run_tests() 
{
    printf("BEGIN TESTING;\n");

    test_create_account();
    test_load_account();
    test_register_transaction();

    printf("END TESTING;\n");
}

int
main(int argc, char *argv[])
{

    run_tests();

    return 0;
}
