#include "db.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sqlite3.h>
#include <time.h>

#include "utils.h"

sqlite3 *db;
struct account *_accounts;
char *path_to_db; 
int arr_size = 0;

void delete_account(char *acc_name)
{
    sqlite3_open(path_to_db, &db);

    int rc;
    char *sql = "DELETE FROM account WHERE name = ?";
    sqlite3_stmt *stmt;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, acc_name, -1, SQLITE_STATIC);
    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

}

void set_db(char *path)
{
    path_to_db = path;
}

void start_empty_db(char *name)
{

    printf("Starting new db...\n");
    
    // Used to store the SQLite error message.
    name = strcat(name, ".db");
    char *error_msg;
    int rc;
    char *sql;

    rc = sqlite3_open(name, &db);

    if (rc != SQLITE_OK) 
    {
        printf("ERROR: %s\n", sqlite3_errmsg(db));
        return;
    }

    sql =   "CREATE TABLE account (\
             id	INTEGER NOT NULL,\
             name	TEXT NOT NULL,\
             balance	REAL NOT NULL,\
             goal REAL,\
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
        printf("ERROR: %s\n", error_msg);
        sqlite3_free(error_msg);
    } 
    else 
    {
        printf("Database has been created.");
    }

    sqlite3_close(db);
    
}


void insert_new_account(struct account *acc)
{
    char *sql;
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_open(path_to_db, &db);

    if (rc != SQLITE_OK) 
    {
        printf("ERROR: \n\t%s\n", 
                sqlite3_errmsg(db));
        return;
    }

    sql = "INSERT INTO ACCOUNT(name, balance, goal)" \
          "VALUES(?, ?, ?);";

    rc = sqlite3_prepare(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) 
    {
        printf("ERROR: %s\n", 
                sqlite3_errmsg(db));
        return; 
    }

    sqlite3_bind_text(
            stmt,               /*statement*/
            1,                  /*index*/
            acc->name,          /*the string*/
            strlen(acc->name),  /*n of bytes of string*/
            SQLITE_TRANSIENT
            );

    sqlite3_bind_double(stmt, 2, acc->balance);
    sqlite3_bind_double(stmt, 3, acc->goal);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) 
    {
        printf("ERROR: Could not add account:\n \t`%s`\n", 
                sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return /* failure */;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

}

void print_account_names()
{

    int rc;
    char *sql = "SELECT * FROM account";
    sqlite3_stmt *stmt;

    rc = sqlite3_open(path_to_db, &db);

    if(rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", sqlite3_errmsg(db));
        return;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if(rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", sqlite3_errmsg(db));
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {

        double balance = sqlite3_column_double(stmt, 2);
        double goal = sqlite3_column_double(stmt, 3);

        int percentage;
            
        if (goal == 0.00)
            percentage = 0;
        else
            percentage = ((int)balance * 100) / (int)goal;
        
        printf("> %s:\n", sqlite3_column_text(stmt, 1));
        printf(" \t£%0.2f | £%0.2f\n", balance, goal); 

        printf("\t["); 
        
        for (int i=0; i<percentage/10; i++)
            printf("#"); 

        for (int i=0; i<10-(percentage/10); i++)
            printf(" "); 

        printf("] %d%\n", percentage); 

    }

    sqlite3_close(db);

}

/*
 * Returns 1 if account exists
 */
int account_exists(char *acc_name)
{
    remove_trailing_chars(acc_name);
    
    sqlite3_stmt *stmt;

    char *sql;
    int rc;
    int exists = 0;

    rc = sqlite3_open_v2(path_to_db, 
            &db, 
            SQLITE_OPEN_READWRITE,
            NULL);

    sql = "SELECT name FROM account\
                 WHERE name = ?";

    if(rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", sqlite3_errmsg(db));
        return 0;
    }


    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if(rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, acc_name, -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) == SQLITE_ROW) 
        exists = 1;

    sqlite3_finalize(stmt);
    rc = sqlite3_close(db);

    if(rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", sqlite3_errmsg(db));
        return 0;
    }


    return exists;

}

void update_account_balance(char *acc_name, double amount)
{

    sqlite3_stmt *stmt;
    char *sql;
    int rc;
    double acc_bal;

    rc = sqlite3_open(path_to_db, &db);

    sql = "SELECT balance FROM account\
           WHERE name = ?;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    sqlite3_bind_text(stmt, 1, acc_name, -1, SQLITE_STATIC);

    if(rc != SQLITE_OK)
        printf("ERROR: Couldn't open database: \n\t %s\n", sqlite3_errmsg(db));

    rc = sqlite3_step(stmt);

    if(rc == SQLITE_ROW)
    {
        acc_bal = sqlite3_column_double(stmt, 0);
    }
    else
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        printf("ERROR: Couldn't get the account balance: Abort...\n");
        return;
    }
        
    sqlite3_finalize(stmt);

    sql = "UPDATE account\
           SET balance = ?\
           WHERE name = ?;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) 
    {
        printf("ERROR: prepare failed: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_double(stmt, 1, acc_bal + amount);

    printf("New balance is: %0.2f\n", acc_bal + amount);

    sqlite3_bind_text(stmt, 2, acc_name, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

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

    rc = sqlite3_open(path_to_db, &db);

    if (rc != SQLITE_OK) 
    {
        printf("ERROR: Opening database: %s\n", rc, sqlite3_errmsg(db));
        return;
    }

    sql = "INSERT INTO movement(account_id,                \
           reference, amount, timestamp)                   \
           VALUES((SELECT id FROM account                  \
                       WHERE name = ?), ?, ?, ?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) 
    {
        printf("ERROR: preparing statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, acc_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, ref, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, amount);
    sqlite3_bind_text(stmt, 4, timestamp, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    update_account_balance(acc_name, amount);
}

struct transaction* load_transactions_for_account(char *acc_name, 
        size_t *size)
{

    int account_id = -1;
    sqlite3_stmt *stmt;
    int transactions_array_size = 0;
    int rc;
    size_t counter = 0;

    rc = sqlite3_open(path_to_db, &db);

    if(rc != SQLITE_OK)
    {
        printf("ERROR: Could not open database: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    char *sql = "SELECT COUNT(*) FROM movement WHERE \
                 account_id = (SELECT id FROM account \
                         WHERE name = ?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if(rc != SQLITE_OK)
    {
        printf("ERROR: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    sqlite3_bind_text(stmt, 1, acc_name, -1, SQLITE_STATIC);

    if((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
        transactions_array_size = sqlite3_column_int(stmt, 0);
    
    struct transaction* transactions = 
        (struct transaction*)malloc(transactions_array_size* 
                sizeof (struct transaction));

    sql = "SELECT * FROM movement WHERE account_id = \
           (SELECT id FROM account WHERE name = ?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, acc_name, -1, SQLITE_STATIC);

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
        printf("ERROR: %s\n ", sqlite3_errmsg(db));

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    *size = transactions_array_size;

    return transactions;
}


