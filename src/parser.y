%{
    #include<stdio.h>
    #include<string.h>
    #include<stdlib.h>
    #include"../headers/symbol-table.h"

    int yylex();
    void yyerror(const char *s);

    int pass; // storing the pass number 1 or 2
    int loc = 0;  // keeping track of the address / location cnt
    extern FILE *yyin;
    SymbolTable *symbol_table;
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

data_section: SEC_DATA { loc = 0; printf("Parsing .data section\n"); } NEWLINE  data_lines 
            ;

data_lines: data_line NEWLINE data_lines 
          | data_line
          |
          ;

data_line: LABEL DEFINE_DATA_TYPE value {  
                                          insert_symbol(symbol_table,$1,loc,DATA_SECTION,$2,DEFINED_SYMBOL,$3); 
                                          loc+=$2 ; 
                                        }
         ;

bss_section: SEC_BSS { loc = 0; printf("Parsing .bss section\n"); } NEWLINE bss_lines
           ;

bss_lines: bss_line NEWLINE bss_lines
         | bss_line
         |
         ;

bss_line: LABEL DECLARE_BSS_TYPE value { printf("BSS: %s %d %d\n", $1, $2 , $3); }
        ;

text_section: SEC_TEXT { loc = 0; printf("Parsing the text section\n"); } NEWLINE text_lines
        ;

text_lines : text_line NEWLINE text_lines
    | text_line
    | 
    ;

text_line : GLOBAL LABEL
    | LABEL_DECLARE {handle_label_declare(symbol_table,$1,loc,DEFINED_SYMBOL);} inst 
    | inst;


inst : 
    // | OPC value {
    //     printf("opcode immediate\n"); 
    // } 
    // | OPC PLUS value {printf("opcode positive immediate\n");}
    // | OPC MINUS value {printf("opcode negative immediate\n");}

    | OPC LABEL {printf("opcode label\n");}
    | OPC REG {printf("opcode register\n");}

    | OPC REG COMMA REG {printf("opcode reg reg\n");}
    | OPC OPENING_BRACKET REG CLOSING_BRACKET COMMA REG {printf("regAddress , reg\n");}
    // | OPC OPENING_BRACKET REG PLUS value CLOSING_BRACKET COMMA REG {printf("regAddress with displacement , reg\n");}
    // | OPC OPENING_BRACKET REG MINUS value CLOSING_BRACKET COMMA REG {printf("regAddress with negative displacement , reg\n");}

    | OPC REG COMMA value {printf("register immediate\n");}
    | OPC REG COMMA PLUS value {printf("Register  positive_immediate\n");}
    | OPC REG COMMA MINUS value {printf("Register negative_immediate\n");}

    | OPC REG COMMA LABEL {printf("Register and variable\n");}
    | OPC REG COMMA OPENING_BRACKET LABEL CLOSING_BRACKET {printf("Register , label adrress");}
    // | OPC REG COMMA OPENING_BRACKET LABEL PLUS value CLOSING_BRACKET {printf("Register , label adrress");}
    // | OPC REG COMMA OPENING_BRACKET LABEL MINUS value CLOSING_BRACKET {printf("Register , label adrress");}

    | OPC REG COMMA OPENING_BRACKET value CLOSING_BRACKET {printf("Register immediate_Adrresing\n");}
    | OPC REG COMMA OPENING_BRACKET REG CLOSING_BRACKET {printf("Register Register Addressing\n");}
    // | OPC REG COMMA OPENING_BRACKET REG PLUS value CLOSING_BRACKET {printf("\n");}
    // | OPC REG COMMA OPENING_BRACKET REG MINUS value CLOSING_BRACKET {printf("\n");}

    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET 
    // | OPC DWORD OPENING_BRACKET REG PLUS value CLOSING_BRACKET 
    // | OPC DWORD OPENING_BRACKET REG MINUS value CLOSING_BRACKET 

    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET COMMA value
    // | OPC DWORD OPENING_BRACKET REG PLUS value CLOSING_BRACKET COMMA value
    // | OPC DWORD OPENING_BRACKET REG MINUS value CLOSING_BRACKET COMMA value
    
    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET COMMA PLUS value
    // | OPC DWORD OPENING_BRACKET REG PLUS value CLOSING_BRACKET COMMA PLUS value
    // | OPC DWORD OPENING_BRACKET REG MINUS value CLOSING_BRACKET COMMA PLUS value

    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET COMMA MINUS value
    // | OPC DWORD OPENING_BRACKET REG PLUS value CLOSING_BRACKET COMMA MINUS value
    // | OPC DWORD OPENING_BRACKET REG MINUS value CLOSING_BRACKET COMMA MINUS value
    ;

value: DEC_VAL { $$ = $1;}
     | HEX_VAL { $$ = $1;}
     | BIN_VAL { $$ = $1;}
     ;
 
%%



void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
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
    display_symbol_table(symbol_table);
}
