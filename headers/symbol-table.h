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
} Symbol;

Symbol *createSymbol(char *name, int address, int section, int size,int status) ;
int insertSymbol(GHashTable *symbolTable, char *name, int address, int section, int size,int status);
Symbol *searchSymbol(GHashTable *symbolTable, char *name);
void displaySymbolTable(GHashTable *symbolTable);
void freeSymbolTable(GHashTable *symbolTable);