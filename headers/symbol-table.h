#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include<glib.h>

#define DEFINED_SYMBOL 1
#define DECLARED_SYMBOL 2
#define UNDEFINED_SYMBOL 3 

#define DATA_SECTION 1
#define BSS_SECTION 2
#define TEXT_SECTION 3

// Structure for a symbol
typedef struct Symbol {
    char *name;
    int address;
    int section;
    int size;
    int status;
    int id;
} Symbol;

// typedef GHashTable SymbolTable;
typedef struct SymbolTable {
    GHashTable *table; // For fast lookup by name
    GList *sorted_list; // For maintaining sorted order by ID
} SymbolTable;


SymbolTable* init_symbol_table();
int is_symbol_exists(SymbolTable *symbol_table,char *symbol_name);
Symbol *create_symbol(char *name, int address, int section, int size,int status, int id) ;
int insert_symbol(SymbolTable *symbol_table, char *name, int address, int section, int size,int status);
Symbol *search_symbol(SymbolTable *symbol_table, char *name);
void display_symbol_table(SymbolTable *symbol_table);
void free_symbol_table(SymbolTable *symbol_table);

int extract_symbols_in_file(SymbolTable *symbol_table,FILE *symbt_file);



#endif 