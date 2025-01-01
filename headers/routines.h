#include"./symbol-table.h"

int handle_variable_symbol(int pass,SymbolTable *symbol_table,char *symbol_name,int address,int section,int size,int status,int value);
void handle_label(int pass,SymbolTable *symbol_table,char *symbol_name,int address,int section,int status);
int handle_op_label(int pass,char* op_name, char* label_name);