%{
    #include<stdio.h>

    int yylex();
    void yyerror(const char *s);

    int pass; // storing the pass number 1 or 2
    int loc;  // keeping track of the address / location cnt
%}


%token SEC_DATA SEC_BSS SEC_TEXT
%token NEWLINE COMMA
%token GLOBAL
%token DWORD
%token OPENING_BRACKET CLOSING_BRACKET
%token <i> DEFINE_DATA_TYPE DECLARE_BSS_TYPE
%token <l> HEX_VAL DEC_VAL BIN_VAL
%token <s> LABEL_DECLARE LABEL REG OPC MEM
%token <l> IMMD



%%union{
    int i,
    long l,
    char* s;
    char c;
}%%

%%
    lines :  line lines
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
    pass = 1;
    yyparse();
}
