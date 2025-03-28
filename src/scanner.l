%{
    #include<stdio.h>
    #include <string.h>
    #include"parser.tab.h"
    #include"../headers/utils.h"
    int yyline = 0;
%}

%option caseless

SECTION "section"
global global
REGS eax|ebx|ecx|edx|esp|ebp|esi|edi
APLHNUM [a-zA-Z_]+[a-zA-Z0-9_]*
SPACE [ \t]+
NUMBER [1-9][0-9]*
COMMENT ;.*

%%

{COMMENT}            { /* Ignore */ }


{SECTION}{SPACE}".data"  {return SEC_DATA;}
{SECTION}{SPACE}".bss"   {return SEC_BSS;}
{SECTION}{SPACE}".text" {return SEC_TEXT;}


d[bwdq]              { yylval.i = get_size_in_bytes(yytext[1]); return DEFINE_DATA_TYPE;}
res[bwdq]            { yylval.i = get_size_in_bytes(yytext[strlen(yytext)-1]); return DECLARE_BSS_TYPE;}

{REGS}                 { yylval.s = to_lowercase_in_place(strndup(yytext,strlen(yytext))); return REG;}

MOV|ADD|SUB|DIV|MUL|XOR|JMP|JZ|JNZ|CMP|INC|DEC|NOT|AND|OR  { yylval.s= to_lowercase_in_place(strndup(yytext,strlen(yytext))); return OPC; }

{global}            { return GLOBAL;}

","                 {return COMMA;}
"["                 {return OPENING_BRACKET;}
"]"                 {return CLOSING_BRACKET;}

\+                  {return PLUS;}
\-                  {return MINUS;}

dword               {return DWORD;}

{NUMBER}            {yylval.i = parsenum(yytext); return DEC_VAL;}

0[xX][0-9a-fA-F]+   {yylval.i = parsenum(yytext); return HEX_VAL;}
0[bB][0-1]+         {yylval.i = parsenum(yytext); return BIN_VAL;}

{APLHNUM}[ \t]*:  { yylval.s = strndup(yytext,strip_extra_chars(yytext)); return LABEL_DECLARE;}
{APLHNUM}          {yylval.s= strndup(yytext,strlen(yytext)); return LABEL;}
\n                  { return NEWLINE;}
[ \t]               { /* ignore white spaces */ }

%%

int yywrap(){ 
    return 1;
}
