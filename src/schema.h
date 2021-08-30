/*
 * The overall schema for this project.
 */
#ifndef SCHEMA_H
#define SCHEMA_H

typedef struct 
{
    int account_id;
    double amount;
    char ref[100];
    char timestamp[100];
} transaction_t;

typedef struct
{
    int id;
    char name[100];
    double balance;
} account_t; 

#endif
