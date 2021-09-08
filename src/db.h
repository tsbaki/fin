#include "schema.h"

void set_db(char *path_to_file);

void insert_new_account(account_t *acc);

void register_transaction(char *ref, char *acc_name, double amount);

transaction_t* load_transactions_for_account(char *acc_name, size_t *size);

int account_exists(char *acc_name);

void delete_account(char *acc_name);

account_t* start_empty_db(char *name);
