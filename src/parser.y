%{
    #include<stdio.h>
    #include<string.h>
    #include<stdlib.h>

    int yylex();
    void yyerror(const char *s);

    int pass; // storing the pass number 1 or 2
    int loc;  // keeping track of the address / location cnt
    extern FILE *yyin;
%}


%token SEC_DATA SEC_BSS SEC_TEXT
%token NEWLINE COMMA
%token GLOBAL
%token DWORD
%token OPENING_BRACKET CLOSING_BRACKET
%token <i> DEFINE_DATA_TYPE DECLARE_BSS_TYPE
%token <l> HEX_VAL DEC_VAL BIN_VAL 
%token <s> LABEL_DECLARE LABEL REG OPC MEM

%type <l> value

%union{
    int i;
    long l;
    char* s;
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
       ;

data_section: SEC_DATA { printf("Parsing .data section\n"); } NEWLINE  data_lines 
            ;

data_lines: data_line NEWLINE data_lines 
          | data_line
          |
          ;

data_line: LABEL DEFINE_DATA_TYPE value { printf("Data: %s %d %ld\n", $1, $2, $3); }
         ;

bss_section: SEC_BSS { printf("Parsing .bss section\n"); } NEWLINE bss_lines
           ;

bss_lines: bss_line NEWLINE bss_lines
         | bss_line
         |
         ;

bss_line: LABEL DECLARE_BSS_TYPE value { printf("BSS: %s %d %ld\n", $1, $2 , $3); }
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
    yyin = fopen(argv[1],"r");
    if(!yyin){
        perror("Error Opening File\n");
        return 0;
    }
    pass = 1;
    yyparse();
}
