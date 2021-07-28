/*
 * The overall schema for this project.
 */
#ifndef SCHEMA_H
#define SCHEMA_H
typedef struct transaction_t
{
    char to[100];
    double amount;
} transaction_t;

typedef struct account_t
{
    char name[100];
    double balance;
    transaction_t transactions[100];
} account_t; 

#endif
