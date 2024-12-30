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
%token GLOBAL
%token DWORD
%token OPENING_BRACKET CLOSING_BRACKET
%token <i> DEFINE_DATA_TYPE DECLARE_BSS_TYPE
%token <i> HEX_VAL DEC_VAL BIN_VAL 
%token <s> LABEL_DECLARE 
%token <s> LABEL REG OPC MEM

%type <l> value

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

data_section: SEC_DATA { printf("Parsing .data section\n"); } NEWLINE  data_lines 
            ;

data_lines: data_line NEWLINE data_lines 
          | data_line
          |
          ;

data_line: LABEL DEFINE_DATA_TYPE value { Symbol *symbol = insert_symbol($1,loc,DATA_SECTION,$2,)}
         ;

bss_section: SEC_BSS { printf("Parsing .bss section\n"); } NEWLINE bss_lines
           ;

bss_lines: bss_line NEWLINE bss_lines
         | bss_line
         |
         ;

bss_line: LABEL DECLARE_BSS_TYPE value { printf("BSS: %s %d %ld\n", $1, $2 , $3); }
        ;

text_section: SEC_TEXT { printf("Parsing the text section\n"); } NEWLINE text_lines
        ;

text_lines : text_line NEWLINE text_lines
    | text_line
    | 
    ;

text_line : GLOBAL LABEL {printf("GLOBAL %s \n",$2);}
    | LABEL_DECLARE {printf("%s ",$1);} inst
    | inst;

inst : OPC {printf("%s\n",$1);}
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
}
