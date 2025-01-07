#include"./symbol-table.h"

int handle_variable_symbol(int pass,SymbolTable *symbol_table,FILE *lst_file,FILE *data_file,char *symbol_name,int address,int section,int size,int status,int value);
void handle_label(int pass,SymbolTable *symbol_table,char *symbol_name,int address,int section,int status);
int handle_op_label(int pass,SymbolTable *symbol_table,FILE *lst_file,FILE* txt,int address,char* op_name, char* label_name);
int handle_op_register(int pass,FILE * lst_file,FILE* txt,char* op_name,char *reg);
int handle_op_reg_reg(int pass,FILE * lst_file,FILE* txt,char *op_name,char *reg1,char*reg2);
int handle_reg_addr_to_reg(int pass,FILE * lst_file,FILE* txt, char *op_name, char *reg1, char *reg2);
int handle_reg_to_immd_val(int pass,FILE * lst_file,FILE* txt,char *op_name,char *reg1,int value);
int handle_reg_to_label(int pass,SymbolTable* symbol_table ,FILE * lst_file,FILE* txt,char *op_name,char *reg1,char *label_name);
int handle_reg_to_label_address(int pass,SymbolTable* symbol_table ,FILE * lst_file,FILE* txt,char* op_name, char *reg1, char* label_name);
int handle_reg_to_immd_address(int pass,FILE * lst_file,FILE* txt,char *op_name,char *reg1,int value);
int handle_reg_to_reg_address(int pass,FILE * lst_file,FILE* txt,char *op_name,char* reg1,char *reg2);
int handle_op_reg_addr(int pass,FILE * lst_file,FILE* txt,char *op_name,char *reg1);
int handle_dword_reg_addr_to_immd(int pass,FILE * lst_file,FILE* txt,char *opc,char *reg1,int value);

