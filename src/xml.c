#include <stdio.h>

#include "../lib/ezxml/ezxml.h"

#include "fin/schema.h"

account_t account;

void 
save_account(account_t *acc)
{
    account = *acc;
}

account_t 
load_account(char *name)
{
    return account;
}

void 
register_transaction(transaction_t *trans)
{
    int amount = trans->amount;
}
