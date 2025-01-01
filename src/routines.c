#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/routines.h"
#include "../headers/symbol-table.h"

int handle_variable_symbol(int pass, SymbolTable *symbol_table, char *symbol_name, int address, int section, int size, int status, int value)
{
    if (pass == 1)
    {

        int res = insert_symbol(symbol_table, symbol_name, address, section, size, status, value);

        if (res == 0)
        {
            exit(EXIT_FAILURE);
        }
        return 1;
    }
}

void handle_label(int pass, SymbolTable *symbol_table, char *symbol_name, int address, int section, int status)
{
    // fecth the symbol
    Symbol *symbol = search_symbol(symbol_table, symbol_name);
    if (pass == 1)
    {

        if (symbol && symbol->section == TEXT_SECTION)
        {
            // Already defined
            if (symbol->status == DEFINED_SYMBOL)
            {
                // Redifining
                if (status == DEFINED_SYMBOL)
                {
                    printf("Error : Redifining Symbol : %s\n", symbol_name);
                    exit(EXIT_FAILURE);
                }
            }
            // symbol present is undefined
            else if (symbol->status == UNDEFINED_SYMBOL)
            {
                // Do nothing
                if (status == UNDEFINED_SYMBOL)
                {
                }
                // update the symbol
                else if (status == DEFINED_SYMBOL)
                {
                    symbol->address = address;
                    symbol->status = DEFINED_SYMBOL;
                }
            }
        }
        else if (!symbol && section == TEXT_SECTION)
        {
            insert_symbol(symbol_table, symbol_name, address, section, 0, status, -1);
        }
        else
        {
            printf("Error : Invalid use of symbol %s\n", symbol_name);
            exit(EXIT_FAILURE);
        }
        return 1;
    }
}

// checks if inst is valid and returns the size of inst
int handle_op_label(int pass,char* op_name, char* label_name)
{
    if (
        strcmp(op_name, "jmp") == 0 ||
        strcmp(op_name, "jz") == 0 ||
        strcmp(op_name, "jnz") == 0 ||
        strcmp(op_name, "JMP") == 0 ||
        strcmp(op_name, "JZ") == 0 ||
        strcmp(op_name, "JNZ") == 0 )
    {
        // assumption is that jmps will be relative 8 bits only
        return 2;
    }
    printf("Error : Invalid use of label : %s\n", label_name);
    exit(EXIT_FAILURE);
}