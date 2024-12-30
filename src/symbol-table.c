#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include"../headers/symbol-table.h"

char * get_section_name(int val){
    switch (val)
    {
    case DATA_SECTION:
        return "data";
        
    case BSS_SECTION :
        return "bss";

    case TEXT_SECTION :
        return "text";
    
    default:
        return "Invalid";
    }
}

char * get_status_name(int val){
    switch (val)
    {
    case DEFINED_SYMBOL:
        return "D";
        
    case UNDEFINED_SYMBOL :
        return "U";

    case DECLARED_SYMBOL :
        return "RES";
    
    default:
        return "Invalid";
    }
}

SymbolTable* init_symbol_table(){
    return (SymbolTable *) g_hash_table_new(g_str_hash,g_str_equal);
}

int is_symbol_exists(SymbolTable *symbolTable,char *symbol_name){
    if (g_hash_table_contains(symbolTable, symbol_name))    return 1;
    return 0;
}

// Function to create a new symbol
Symbol *createSymbol(char *name, int address, int section, int size,int status) {
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->name = g_strdup(name);  
    symbol->address = address;
    symbol->section = section;
    symbol->size = size;
    symbol->status = status;
    return symbol;
}

// Insert a symbol into the table
int insertSymbol(SymbolTable *symbolTable, char *name, int address, int section, int size,int status) {
    char *key = name;
    if (g_hash_table_contains(symbolTable, key)) {
        printf("Error: Symbol '%s' already exists in section %d'.\n", name, section);
        return 0;
    }

    Symbol *symbol = createSymbol(name, address, section, size,status);
    g_hash_table_insert(symbolTable, g_strdup(key), symbol);

    return 1;
}

// Search for a symbol in the table
Symbol *searchSymbol(SymbolTable *symbolTable, char *name) {
    return (Symbol *)g_hash_table_lookup(symbolTable, name);
}

// Display the symbol table
void displaySymbolTable(SymbolTable *symbolTable) {
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, symbolTable);

    printf("Symbol Table:\n");
    printf("Name\tAddress\tSection\tType\tStatus\n");
    printf("-----------------------------------\n");

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Symbol *symbol = (Symbol *)value;
        printf("%s\t%d\t%s\t%d\t%s\n", symbol->name, symbol->address, get_section_name(symbol->section), symbol->size,get_status_name(symbol->status));
    }
}

// Free allocated memory
void freeSymbolTable(SymbolTable *symbolTable) {
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, symbolTable);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Symbol *symbol = (Symbol *)value;
        free(symbol->name);  // Free memory allocated for the symbol name
        free(symbol);  // Free the symbol structure itself
        g_free(key);  // Free the memory allocated for the hash key
    }
    g_hash_table_destroy(symbolTable);
}
