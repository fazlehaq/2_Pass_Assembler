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

int is_symbol_exists(SymbolTable *symbol_table,char *symbol_name){
    if (g_hash_table_contains(symbol_table, symbol_name))    return 1;
    return 0;
}

// Function to create a new symbol
Symbol *create_symbol(char *name, int address, int section, int size,int status,int value) {
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->name = g_strdup(name);  
    symbol->address = address;
    symbol->section = section;
    symbol->size = size;
    symbol->status = status;
    symbol->value = value;
    return symbol;
}

// Insert a symbol into the table
int insert_symbol(SymbolTable *symbol_table, char *name, int address, int section, int size,int status,int value) {
    char *key = name;
    if (g_hash_table_contains(symbol_table, key)) {
        printf("Error: Symbol '%s' already exists in section %d'.\n", name, section);
        return 0;
    }

    Symbol *symbol = create_symbol(name, address, section, size,status,value);
    g_hash_table_insert(symbol_table, g_strdup(key), symbol);

    return 1;
}

// Search for a symbol in the table
Symbol *search_symbol(SymbolTable *symbol_table, char *name) {
    return (Symbol *)g_hash_table_lookup(symbol_table, name);
}

// Display the symbol table
void display_symbol_table(SymbolTable *symbol_table) {
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, symbol_table);

    printf("Symbol Table:\n");
    printf("Name\tAddress\tSection\tType\tValue\tStatus\n");
    printf("---------------------------------------------------\n");

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Symbol *symbol = (Symbol *)value;
        printf("%s\t%X\t%s\t%d\t%X\t%s\n", symbol->name, symbol->address, get_section_name(symbol->section), symbol->size,symbol->value,get_status_name(symbol->status));
    }
}

// Free allocated memory
void free_symbol_table(SymbolTable *symbol_table) {
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, symbol_table);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Symbol *symbol = (Symbol *)value;
        free(symbol->name);  // Free memory allocated for the symbol name
        free(symbol);  // Free the symbol structure itself
        g_free(key);  // Free the memory allocated for the hash key
    }
    g_hash_table_destroy(symbol_table);
}
