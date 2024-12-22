%{
    #include<stdio.h>
    #include <string.h>
    #include"parser.tab.h"
%}

%option caseless

SECTION "section"
global global$
REGS (eax|ebx|ecx|edx|esp|ebp|esi|edi)
APLHNUM [a-zA-Z_]+[a-zA-Z0-9_]*
SPACE [ \t]+
NUMBER [1-9][0-9]*
COMMENT ;.*

%%

{COMMENT}            { /* Ignore */ }

{SECTION}{SPACE}".data"  { return SEC_DATA;}
{SECTION}{SPACE}".bss"   {return SEC_BSS;}
{SECTION}{SPACE}".text" {return SEC_TEXT;}


d[bwdq]              { return DEFINE_DATA; }
res[bwdq]            { return DECLARE_BSS;}

REGS                 { return REG;}

MOV|ADD|SUB|DIV|MUL|XOR|JMP|JZ|JNZ|CMP|INC|DEC|NOT|AND|OR  { return OPC; }

{global}            { return GLOBAL;}

","                 {return COMMA;}
"["                 {return OPENING_BRACKET;}
"]"                 {return CLOSING_BRACKET;}

dword               {return DWORD;}

{NUMBER}            {yylval = atol(yytext); return DEC_VAL;}
0[xX][0-9a-fA-F]+   {yylval = yytext; return HEX_VAL;}
0[bB][0-1]+         {yylval = yytext; return BIN_VAL;}

{APLHNUM}{SPACE}:$  { yylval = yytext; return LABEL_DECLARE;}
{APLHNUM}$          { yylval = yytext; return LABEL;}

\n                  {return NEWLINE;}
[ \t]               { /* ignore white spaces */ }

%%

int yywrap(){ 
    return 1;
}
