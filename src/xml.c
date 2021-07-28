#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <libxml/encoding.h>

#include "fin/schema.h"

#define ENCODING "UTF-8"

void save_to_file(xmlDoc *doc, char *file_name)
{
    xmlChar *xmlbuff;
    int buffersize;
    FILE *fp;

    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);

    fp = fopen(file_name, "r+");
    fprintf(fp, xmlbuff);
    fclose(fp);
}

/*
 * Checks if file exists. If doesn't, then it will
 * create one.
 */
bool check_file_exists(char *path_to_file)
{
    if(access(path_to_file, F_OK) < 0)
        return 0;
    else
        return 1;
}

void setup_new_xml()
{
    FILE *fp;
    char *user_home = getenv("HOME");
    char *file_name = strcat(user_home, "/default.fin");

    if(check_file_exists(file_name)) 
    {
        printf("Default file already exists: %s\n", file_name);
        return;
    }

    fp = fopen(file_name, "w+");

    fprintf(fp,"<?xml version=\"1.0\"?>\n");
    fprintf(fp,"%s", "<accounts></accounts>\n");

    printf("'default.fin' has been created in %s\n", file_name);

    fclose(fp);
}

void create_account(account_t *acc, char *path_to_file)
{
    // Check if file exists
    if(!check_file_exists(path_to_file))
    {
        // TODO If the file does not exist then the program
        // will tell the user so. It should tell the user
        // to use the specified to command to initialize an
        // empty file.
        printf("Cannot access '%s': No such file.\n", path_to_file);
        return;
    } 

    xmlDoc *doc = NULL;
    xmlNode *accounts_root_node= NULL;
    xmlNode *new_account_node = NULL;
    xmlNode *new_balance_node = NULL;
    xmlNode *new_transactions_node = NULL;

    xmlChar *acc_name = BAD_CAST acc->name;

    char bal_to_cast[100];
    sprintf(bal_to_cast, "%lf", acc->balance);

    xmlChar *acc_balance = BAD_CAST bal_to_cast;

    doc = xmlReadFile(path_to_file, NULL, 0);

    if (doc == NULL) 
    {
        printf("Could not parse the XML file.\n");
        return;
    }

    accounts_root_node = xmlDocGetRootElement(doc);
    new_account_node = xmlNewNode(NULL, BAD_CAST "account");
    new_balance_node = xmlNewNode(NULL, BAD_CAST "balance");
    new_transactions_node = xmlNewNode(NULL, BAD_CAST "transactions");

    xmlAddChild(accounts_root_node, new_account_node);
    xmlSetProp(new_account_node, "name", acc_name);

    // Add balance node
    xmlAddChild(new_account_node, new_balance_node);
    //TODO this might not work
    xmlNodeSetContent(new_balance_node, acc_balance);

    // Add transactions node
    xmlAddChild(new_account_node, new_transactions_node);
    //TODO Loop through transactions and add them to node

    save_to_file(doc, path_to_file);
}

void print_account_names(char *path_to_file)
{
    printf("Printing accounts for: %s\n", path_to_file);

    xmlTextReaderPtr reader = xmlReaderForFile(path_to_file, NULL, 0);
    const xmlChar *name;
    int ret = xmlTextReaderRead(reader);

    bool skip = true;

    while(ret == 1)
    {
        if(xmlTextReaderHasAttributes(reader))
        {
            // FIXME: Atm the skip flag is used to skip
            // duplicates. Needs refactoring, a better way
            // to manage this somehow.
            name = xmlTextReaderGetAttribute(reader, "name");

            if(!skip)
                printf(" > %s\n", name);

            skip = !skip;
        }

        //TODO Let the user know if there are no accounts to
        // display.

        ret = xmlTextReaderRead(reader);
    }

    // Close the stream
    xmlFreeTextReader(reader);
}
