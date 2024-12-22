%{
    #include<stdio.h>

    int yylex();
    void yyerror(const char *s);
%}

%token SEC_DATA SEC_BSS SEC_TEXT
%token NEWLINE COMMA
%token GLOBAL
%token DWORD
%token OPENING_BRACKET CLOSING_BRACKET
%token DEFINE_DATA DECLARED_BSS
%token HEX_VAL DEC_VAL BIN_VAL
%token LABEL_DECLARE LABEL REG OPC MEM
%token IMMD


%%
    lines : 
          ;
%%


void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main() {
    return yyparse();
}
