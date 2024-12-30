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
int is_symbol_exists(SymbolTable *symbolTable,char *symbol_name);
Symbol *createSymbol(char *name, int address, int section, int size,int status,int value) ;
int insertSymbol(SymbolTable *symbolTable, char *name, int address, int section, int size,int status,int value);
Symbol *searchSymbol(SymbolTable *symbolTable, char *name);
void displaySymbolTable(SymbolTable *symbolTable);
void freeSymbolTable(SymbolTable *symbolTable);