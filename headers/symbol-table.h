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
    int value;
} Symbol;

typedef GHashTable SymbolTable;

SymbolTable* init_symbol_table();
int is_symbol_exists(SymbolTable *symbol_table,char *symbol_name);
Symbol *create_symbol(char *name, int address, int section, int size,int status,int value) ;
int insert_symbol(SymbolTable *symbol_table, char *name, int address, int section, int size,int status,int value);
Symbol *search_symbol(SymbolTable *symbol_table, char *name);
void display_symbol_table(SymbolTable *symbol_table);
void free_symbol_table(SymbolTable *symbol_table);