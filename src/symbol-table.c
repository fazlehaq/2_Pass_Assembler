#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include"../headers/symbol-table.h"
#include"../headers/obj_file.h"

static int symbol_id_counter = 1; 

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
    SymbolTable *symbol_table = (SymbolTable *)malloc(sizeof(SymbolTable));
    symbol_table->table = g_hash_table_new(g_str_hash, g_str_equal);  // Hash table
    symbol_table->sorted_list = NULL;  // Sorted list initially empty
    return symbol_table;
}

int is_symbol_exists(SymbolTable *symbol_table,char *symbol_name){
    if (g_hash_table_contains(symbol_table->table, symbol_name))    return 1;
    return 0;
}

// Function to create a new symbol
Symbol *create_symbol(char *name, int address, int section, int size, int status, int id) {
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->name = g_strdup(name);
    symbol->address = address;
    symbol->section = section;
    symbol->size = size;
    symbol->status = status;
    symbol->id = id;  
    return symbol;
}

gint compare_symbols_by_id(const Symbol *a, const Symbol *b) {
    return a->id - b->id;
}


// Insert a symbol into the table
int insert_symbol(SymbolTable *symbol_table, char *name, int address, int section, int size, int status) {
    if (g_hash_table_contains(symbol_table->table, name)) {
        printf("Error: Symbol '%s' already exists in section %d'.\n", name, section);
        return 0;
    }

    Symbol *symbol = create_symbol(name, address, section, size, status, symbol_id_counter++);
    g_hash_table_insert(symbol_table->table, g_strdup(name), symbol);

    symbol_table->sorted_list = g_list_insert_sorted(
        symbol_table->sorted_list, symbol, (GCompareFunc)compare_symbols_by_id);

    return 1;
}



// Search for a symbol in the table
Symbol *search_symbol(SymbolTable *symbol_table, char *name) {
    return (Symbol *)g_hash_table_lookup(symbol_table->table, name);
}

// Display the symbol table
void display_symbol_table(SymbolTable *symbol_table) {
    printf("ID\tName\tSection\tStatus\tAddress\tSize(B)\n");
    printf("-----------------------------------------------------------\n");

    GList *node = symbol_table->sorted_list;
    while (node != NULL) {
        Symbol *symbol = (Symbol *)node->data;
        printf("%d\t%s\t%s\t%s\t%X\t%d\n",
            symbol->id, symbol->name, get_section_name(symbol->section),get_status_name(symbol->status),symbol->address
            , symbol->size);
        node = node->next;
    }
}


// Free allocated memory
void free_symbol_table(SymbolTable *symbol_table) {
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, symbol_table->table);

    
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Symbol *symbol = (Symbol *)value;
        free(symbol->name);
        free(symbol);
        g_free(key);
    }
    g_hash_table_destroy(symbol_table->table); 

    g_list_free(symbol_table->sorted_list); 
    free(symbol_table);
}


int extract_symbols_in_file(SymbolTable *symbol_table, FILE *symbt_file) {
    GList *node = symbol_table->sorted_list;
    int symbol_section_size = 0;

    while (node != NULL) {
        Symbol *symbol = (Symbol *)node->data;
        SymbolEntry *entry = (SymbolEntry *) malloc(sizeof(SymbolEntry));

        memset(entry->name, 0, sizeof(entry->name)); 
        strncpy(entry->name, symbol->name, sizeof(entry->name) - 1); 
        entry->section = (char)symbol->section;
        entry->status = (char)symbol->status;
        entry->address = symbol->address;

        fwrite(entry, sizeof(SymbolEntry), 1, symbt_file);
        node = node->next;
        symbol_section_size += sizeof(SymbolEntry);
    }
    return symbol_section_size;
}