#include <stddef.h>

struct transaction {
    int account_id;
    double amount;
    char ref[100];
    char timestamp[100];
};

struct account {
    int id;
    char name[100];
    double balance;
    double goal;
};

void set_db(char *path_to_file);

void insert_new_account(struct account *acc);

void register_transaction(char *ref, char *acc_name, double amount);

struct transaction* load_transactions_for_account(char *acc_name, size_t *size);

int account_exists(char *acc_name);

void delete_account(char *acc_name);

void start_empty_db(char *name);
