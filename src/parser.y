%{
    #include<stdio.h>
    #include<string.h>
    #include<stdlib.h>
    #include"../headers/symbol-table.h"
    #include"../headers/routines.h"

    int yylex();
    void yyerror(const char *s);

    int pass; // storing the pass number 1 or 2
    int loc = 0;  // keeping track of the address / location cnt
    FILE *lst_file;
    extern FILE *yyin;
    extern int yyline;
    SymbolTable *symbol_table;

    void newline(){
        if(pass == 2){
            fprintf(lst_file,"\n");
        }
    }
%}

%token SEC_DATA SEC_BSS SEC_TEXT
%token NEWLINE COMMA
%token PLUS MINUS
%token GLOBAL
%token DWORD
%token OPENING_BRACKET CLOSING_BRACKET
%token <i> DEFINE_DATA_TYPE DECLARE_BSS_TYPE
%token <i> HEX_VAL DEC_VAL BIN_VAL 
%token <s> LABEL_DECLARE 
%token <s> LABEL REG OPC MEM

%type <i> value

%union{
    int i;
    long l;
    char *s;
}


%%
program: section_list
       | /* empty */ { printf("Empty program\n"); }
       ;

section_list:section section_list
            | section
            ;

section: data_section
       | bss_section
       | text_section
       ;

data_section: SEC_DATA { loc = 0; } data_lines 
            ;

data_lines: data_line NEWLINE {newline();} data_lines 
          | NEWLINE {newline();} data_lines
          | data_line
          |
          ;

data_line: LABEL DEFINE_DATA_TYPE value {  
        if(pass == 2)
          fprintf(lst_file,"%d %08X ",yyline,loc);
        handle_variable_symbol(pass,symbol_table,lst_file,$1,loc,DATA_SECTION,$2,DEFINED_SYMBOL,$3);
        loc += $2;
    }
    ;

bss_section: SEC_BSS { loc = 0;  }  bss_lines
           ;

bss_lines: bss_line NEWLINE {newline();} bss_lines
         | NEWLINE{newline();} bss_lines
         | bss_line
         |
         ;

bss_line: LABEL DECLARE_BSS_TYPE value { 
    if(pass == 2)
        fprintf(lst_file,"%d %08X ",yyline,loc);
    handle_variable_symbol(pass,symbol_table,lst_file,$1,loc,BSS_SECTION,$2*$3,DECLARED_SYMBOL,0); loc += ($2*$3);
    }
    ;

text_section: SEC_TEXT { loc = 0; } text_lines
        ;

text_lines : text_line NEWLINE {newline();} text_lines
    | NEWLINE {newline();} text_lines
    | text_line
    |
    ;
 
text_line : GLOBAL LABEL 
    | LABEL_DECLARE {handle_label(pass,symbol_table,$1,loc,TEXT_SECTION,DEFINED_SYMBOL);} inst 
    | inst;
 
 
inst :  
    OPC LABEL { 
        if(pass == 2)
            fprintf(lst_file,"%d %08X ",yyline,loc);
        int size = handle_op_label(pass,symbol_table,lst_file,loc,$1,$2);
        handle_label(pass,symbol_table,$2,-1,TEXT_SECTION,UNDEFINED_SYMBOL);
        loc += size;
    }

    | OPC REG {
        if(pass == 2)
            fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_op_register(pass,lst_file,$1,$2);
    }

    | OPC REG COMMA REG {
        if(pass == 2)
            fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_op_reg_reg(pass,lst_file,$1,$2,$4);
    }
    | OPC OPENING_BRACKET REG CLOSING_BRACKET COMMA REG {
        if(pass == 2)        
            fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_reg_addr_to_reg(pass,lst_file,$1,$3,$6);
    }

    | OPC REG COMMA value {
        if(pass == 2)        
            fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_reg_to_immd_val(pass,lst_file,$1,$2,$4);
    }
    | OPC REG COMMA PLUS value {
        if(pass == 2)        
        fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_reg_to_immd_val(pass,lst_file,$1,$2,$5);
    }
    | OPC REG COMMA MINUS value {
        if(pass == 2)        
        fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_reg_to_immd_val(pass,lst_file,$1,$2,-1 * $5);
    }

    | OPC REG COMMA LABEL {
        Symbol *symbol = search_symbol(symbol_table,$4);
        
        if(!symbol) {
            exit(EXIT_FAILURE);
        }
        if(pass == 2)        
        fprintf(lst_file,"%d %08X ",yyline,loc);
        loc +=handle_reg_to_label(pass,symbol_table,lst_file,$1,$2,$4);
    }
    | OPC REG COMMA OPENING_BRACKET LABEL CLOSING_BRACKET {
        
        Symbol *symbol = search_symbol(symbol_table,$5);
        if(!symbol) {
            printf("Error : Undefined symbol %s\n",$5);
            exit(EXIT_FAILURE);
        }
        if(pass == 2)        
        fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_reg_to_label_address(pass,symbol_table,lst_file,$1,$2,$5);
    }
    
    | OPC REG COMMA OPENING_BRACKET value CLOSING_BRACKET {
        if(pass == 2)        
        fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_reg_to_immd_address(pass,lst_file,$1,$2,$5);
    }
    | OPC REG COMMA OPENING_BRACKET REG CLOSING_BRACKET {
        if(pass == 2)        
        fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_reg_to_reg_address(pass,lst_file,$1,$2,$5);
    }
 
    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET {
        if(pass == 2)        
        fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_op_reg_addr(pass,lst_file,$1,$4);
    }
  
    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET COMMA value {
        if(pass == 2)        
        fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_dword_reg_addr_to_immd(pass,lst_file,$1,$4,$7);
    }
  
    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET COMMA PLUS value{
        if(pass == 2)        
        fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_dword_reg_addr_to_immd(pass,lst_file,$1,$4,$8);
    }

    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET COMMA MINUS value{
        if(pass == 2)        
        fprintf(lst_file,"%d %08X ",yyline,loc);
        loc += handle_dword_reg_addr_to_immd(pass,lst_file,$1,$4,$8);
    }
    ;

value: DEC_VAL { $$ = $1;}
     | HEX_VAL { $$ = $1;}
     | BIN_VAL { $$ = $1;}
     ;
 
%%



void yyerror(const char *s) {
    fprintf(stderr, "Error: %s at %d\n", s,yyline);
}

int main(int argc,char *argv[]) {
    if (argc < 2){
        printf("Error : File not provided!\n");
        return 0;
    }

    // Initializing the symbol table
    symbol_table = init_symbol_table();
    yyin = fopen(argv[1],"r");
    if(!yyin){
        perror("Error Opening File\n");
        return 0;
    }
    pass = 1;
    yyparse();
    fclose(yyin);
    display_symbol_table(symbol_table);
    pass = 2;
    yyline = 1;
    yyin = fopen(argv[1],"r");
    lst_file = fopen("out.lst","w");
    yyparse();
}
