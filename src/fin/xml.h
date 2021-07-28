#include "schema.h"

void create_account(account_t *acc, char *path_to_file);

void register_transaction(transaction_t *trans);

void print_account_names(char *path_to_file);

/*
 * Creates a new xml file to store data, at the specified
 * path
 */
void setup_new_xml();
