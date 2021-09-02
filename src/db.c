#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>

#include "schema.h"
#include "utils.h"

#define MAX_ACCOUNTS 20

sqlite3 *db;
account_t _accounts[MAX_ACCOUNTS];

int array_counter = 0;

static int load_accounts_cb(void *NotUsed, int argc, char **argv, char **azColName)  
{

    for(int i=0; i < argc; i++)  
    {
      if(strcmp(azColName[i],"name") == 0) 
          strcpy(_accounts[array_counter].name, argv[i]);
      if(strcmp(azColName[i],"balance") == 0) 
          _accounts[array_counter].balance = atof(argv[i]);
      if(strcmp(azColName[i],"id") == 0)  
          _accounts[array_counter].id = atol(argv[i]);

    }

    array_counter++;

    return 0;
}

void set_db(char *path)
{
    sqlite3_open(path, &db);

    char *sql;
    char *errmsg = NULL;
    sqlite3_stmt *stmt;

    sql = "SELECT id, name, balance FROM account;";

    sqlite3_exec(db, sql, load_accounts_cb, 0, &errmsg);

    if(errmsg != NULL)
        printf("Error loading accounts:\n%s\n", errmsg);

}


void start_empty_db(char *name)
{
    // Used to store the SQLite error message.
    name = strcat(name, ".db");
    char *error_msg;
    int rc;
    char *sql;

    rc = sqlite3_open(name, &db);

    if(rc) {
        fprintf(stdout, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    sql =   "CREATE TABLE account (\
             id	INTEGER NOT NULL,\
             name	TEXT NOT NULL,\
             balance	REAL NOT NULL,\
             PRIMARY KEY(id AUTOINCREMENT)\
             );\
             CREATE TABLE movement (\
             account_id	INTEGER NOT NULL,\
             amount	REAL NOT NULL,\
             timestamp	TEXT NOT NULL ,\
             reference	TEXT,\
             FOREIGN KEY(account_id) REFERENCES\
             account(id) ON DELETE CASCADE);";

    rc = sqlite3_exec(db, sql, NULL, 0, &error_msg);

    if(rc != SQLITE_OK)
    {
        fprintf(stdout, "SQL error: %s\n", error_msg);
        sqlite3_free(error_msg);
    } 
    else 
    {
        fprintf(stdout, "New db created.\n");
    }

    sqlite3_close(db);

}


void insert_new_account(account_t *acc)
{
    char *sql;
    sqlite3_stmt *stmt;
    int rc;

    sql = "INSERT INTO ACCOUNT(name, balance)" \
          "VALUES(?, ?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) 
    {
        printf("Preparing statement failed: %s\n", 
                sqlite3_errmsg(db));
        return /* failure */;
    }

    sqlite3_bind_text(
            stmt,               /*statement*/
            1,                  /*index*/
            acc->name,          /*the string*/
            strlen(acc->name),  /*n of bytes of string*/
            SQLITE_TRANSIENT
            );

    sqlite3_bind_double(stmt, 2, acc->balance);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) 
    {
        printf("Could not add account:\n \t`%s`\n", 
                sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return /* failure */;
    }

    sqlite3_finalize(stmt);

}

void print_account_names()
{
    char *sql;
    char *errmsg = NULL;
    sqlite3_stmt *stmt;
    int rc;

    sql = "SELECT name, balance FROM account;";

    //@@@
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
          printf("> %s ", sqlite3_column_text(stmt, 0));
          printf("($%lf)\n", sqlite3_column_double(stmt, 1));
    }

    if (errmsg != NULL)
        printf("%s\n", errmsg);

    sqlite3_finalize(stmt);

}

/*
 * Returns 1 if account exists
 */
int account_exists(char *acc_name)
{
    remove_trailing_chars(acc_name);

    for(int i=0; i < MAX_ACCOUNTS; i++) 
        if(strcmp(_accounts[i].name, acc_name)==0)
            return 1;

    return 0;
}

void register_transaction(char *ref, char *acc_name, 
        double amount)
{
    char *sql;
    sqlite3_stmt *stmt;
    int rc;

    char timestamp[20];
    time_t now = time(NULL);
    strftime(timestamp, 20, "%d-%m-%Y", localtime(&now));

    sql = "INSERT INTO movement(account_id,                \
           reference, amount, timestamp)                   \
           VALUES((SELECT id FROM account                  \
                       WHERE name = ?), ?, ?, ?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);


    if (rc != SQLITE_OK) 
    {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, acc_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, ref, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, amount);
    sqlite3_bind_text(stmt, 4, timestamp, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) 
    {
        printf("Error adding transaction:\n \t`%s`\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    // Update the account balance
    sql = "UPDATE account\
           SET balance = ?\
           WHERE name = ?;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) 
    {
        printf("prepare failed: %s\n", sqlite3_errmsg(db));
        return;
    }

    for(int i=0; i<MAX_ACCOUNTS; i++) 
    {
        if(strcmp(_accounts[i].name, acc_name) == 0)
        {
            double new_balance = _accounts[i].balance + amount;

            sqlite3_bind_double(stmt, 1, new_balance);
            sqlite3_bind_text(stmt, 2, acc_name, -1, 
                    SQLITE_STATIC);
        }
    }

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) 
    {
        printf("execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

}

transaction_t* load_transactions_for_account(char *acc_name, 
        size_t *size)
{

    int account_id = -1;
    sqlite3_stmt *stmt;
    int arr_size= 0;
    size_t counter = 0;

    char *sql = "SELECT COUNT(*)  FROM movement WHERE account_id = ?";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if(rc != SQLITE_OK)
    {
        printf("error: ", sqlite3_errmsg(db));
        return NULL;
    }

    /* Find the associated account id using the name. */
    for(int i=0; i<MAX_ACCOUNTS; i++) 
    {
        if(strcmp(_accounts[i].name, acc_name) == 0) 
        {
            account_id = _accounts[i].id;
            break;
        }
    }

    sqlite3_bind_int(stmt, 1, account_id);

    rc = sqlite3_step(stmt);

    if(rc == SQLITE_ROW) 
        arr_size = sqlite3_column_int(stmt, 0);
    
    transaction_t *transactions = 
        (transaction_t*)malloc(arr_size*sizeof(transaction_t));

    sql = "SELECT * FROM movement WHERE account_id = ?";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, account_id);

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        
        transactions[counter].account_id = sqlite3_column_int(stmt, 0);
        transactions[counter].amount = sqlite3_column_double(stmt, 1);

        const char *timestamp = sqlite3_column_text(stmt, 2);
        const char *ref = sqlite3_column_text(stmt, 3);

        strcpy(transactions[counter].timestamp, timestamp);
        strcpy(transactions[counter].ref, ref);
        counter++;
    }

    if(rc != SQLITE_DONE)
        printf("error: ", sqlite3_errmsg(db));

    sqlite3_finalize(stmt);
    *size = arr_size;

    return transactions;
}
