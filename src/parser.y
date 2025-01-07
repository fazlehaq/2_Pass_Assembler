%{
    #include<stdio.h>
    #include<string.h>
    #include<stdlib.h>
    #include"../headers/symbol-table.h"
    #include"../headers/routines.h"
    #include"../headers/obj_file.h"
    #include"../headers/utils.h"

    int yylex();
    void yyerror(const char *s);

    int pass; // storing the pass number 1 or 2
    int loc = 0;  // keeping track of the address / location cnt
    extern FILE *yyin;
    extern int yyline;
    SymbolTable *symbol_table;

    FILE *lst_file;
    // intermediate files
    FILE * obj_file; 
    FILE *txt; // contains text section machine code in binary format
    FILE *data_file; // contains data section in binary format
    FILE *symbt_file;

    SectionHeader section_headers[3];
    SectionHeader *text_header;
    SectionHeader *data_header;
    SectionHeader *symbol_table_header;

    void newline(){
        yyline++;
        if(pass == 2){
                fprintf(lst_file,"\n%d "  ,yyline);
        }
    }
%}

%token SEC_DATA SEC_BSS SEC_TEXT
%token EMPTYLINE NEWLINE COMMA
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

data_section: SEC_DATA {if (yyline == 0) newline(); loc = 0; } data_lines 
            ;

data_lines: data_line NEWLINETYPE data_lines 
          | NEWLINETYPE data_lines
          | data_line
          |
          ;

data_line: LABEL DEFINE_DATA_TYPE value {  
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            else 
                fprintf(lst_file,"%08X ",loc);
                
            data_header->size += $2;
        }
        handle_variable_symbol(pass,symbol_table,lst_file,data_file,$1,loc,DATA_SECTION,$2,DEFINED_SYMBOL,$3);
        loc += $2;
    }
    ;

bss_section: SEC_BSS {if (yyline == 0) newline(); loc = 0;  }  bss_lines
           ;

bss_lines: bss_line NEWLINETYPE bss_lines
         | NEWLINETYPE bss_lines
         | bss_line
         |
         ;

bss_line: LABEL DECLARE_BSS_TYPE value { 
    if(pass == 2){
        if(yyline == 1){
            fprintf(lst_file,"%d %08X ",yyline,loc);
        }
        else {
            fprintf(lst_file,"%08X ",loc);
        }
    }
        handle_variable_symbol(pass,symbol_table,lst_file,data_file,$1,loc,BSS_SECTION,$2*$3,DECLARED_SYMBOL,0); loc += ($2*$3);
    }
    ;

text_section: SEC_TEXT { if (yyline == 0) newline();loc = 0; } text_lines
        ;

text_lines : text_line NEWLINETYPE text_lines
    | NEWLINETYPE text_lines
    | text_line
    |
    ;
 
text_line : GLOBAL LABEL 
    | LABEL_DECLARE {handle_label(pass,symbol_table,$1,loc,TEXT_SECTION,DEFINED_SYMBOL);} inst 
    | inst;
 
 
inst :  
    OPC LABEL { 
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_op_label(pass,symbol_table,lst_file,txt,loc,$1,$2);
        handle_label(pass,symbol_table,$2,-1,TEXT_SECTION,UNDEFINED_SYMBOL);
        loc += size;
        if (pass == 2)
        text_header->size += size;
    }

    | OPC REG {
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_op_register(pass,lst_file,txt,$1,$2);
         loc += size;
        if (pass == 2)
        text_header->size += size;
    }

    | OPC REG COMMA REG {
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_op_reg_reg(pass,lst_file,txt,$1,$2,$4);
        loc += size;
        if (pass == 2)
        text_header->size += size;
        
    }
    | OPC OPENING_BRACKET REG CLOSING_BRACKET COMMA REG {
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_reg_addr_to_reg(pass,lst_file,txt,$1,$3,$6);
        loc += size;

        if (pass == 2)
        text_header->size += size;
    }

    | OPC REG COMMA value {
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_reg_to_immd_val(pass,lst_file,txt,$1,$2,$4);
        loc += size;
        
        if (pass == 2)
        text_header->size += size;

    }
    | OPC REG COMMA PLUS value {
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_reg_to_immd_val(pass,lst_file,txt,$1,$2,$5);
        loc += size;
        if (pass == 2)
        text_header->size += size;
    }
    | OPC REG COMMA MINUS value {
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size= handle_reg_to_immd_val(pass,lst_file,txt,$1,$2,-1 * $5);
        loc += size;
        if (pass == 2)
        text_header->size += size;
    }

    | OPC REG COMMA LABEL {
        Symbol *symbol = search_symbol(symbol_table,$4);
        
        if(!symbol) {
            exit(EXIT_FAILURE);
        }
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size=handle_reg_to_label(pass,symbol_table,lst_file,txt,$1,$2,$4);
        loc += size;
        if (pass == 2)
        text_header->size += size;
    }
    | OPC REG COMMA OPENING_BRACKET LABEL CLOSING_BRACKET {
        
        Symbol *symbol = search_symbol(symbol_table,$5);
        if(!symbol) {
            printf("Error : Undefined symbol %s\n",$5);
            exit(EXIT_FAILURE);
        }
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_reg_to_label_address(pass,symbol_table,lst_file,txt,$1,$2,$5);
        loc += size;
        if (pass == 2)
        text_header->size += size;
    }
    
    | OPC REG COMMA OPENING_BRACKET value CLOSING_BRACKET {
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_reg_to_immd_address(pass,lst_file,txt,$1,$2,$5);
        loc += size;
        if (pass == 2)
        text_header->size += size;
    }
    | OPC REG COMMA OPENING_BRACKET REG CLOSING_BRACKET {
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_reg_to_reg_address(pass,lst_file,txt,$1,$2,$5);
        loc += size;
        if (pass == 2)
        text_header->size += size;
    }
 
    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET {
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_op_reg_addr(pass,lst_file,txt,$1,$4);
        loc += size;
        if (pass == 2)
        text_header->size += size;
    }
  
    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET COMMA value {
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_dword_reg_addr_to_immd(pass,lst_file,txt,$1,$4,$7);
        loc += size;
        if (pass == 2)
        text_header->size += size;
    }
  
    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET COMMA PLUS value{
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_dword_reg_addr_to_immd(pass,lst_file,txt,$1,$4,$8);
        loc += size;
        if (pass == 2)
        text_header->size += size;
    }

    | OPC DWORD OPENING_BRACKET REG CLOSING_BRACKET COMMA MINUS value{
        if(pass == 2){
            if(yyline == 1)
                fprintf(lst_file,"%d %08X ",yyline,loc);
            fprintf(lst_file,"%08X ",loc);
        }
        int size = handle_dword_reg_addr_to_immd(pass,lst_file,txt,$1,$4,$8);
        loc += size;
        if (pass == 2)
        text_header->size += size;
    }
    ;

value: DEC_VAL { $$ = $1;}
     | HEX_VAL { $$ = $1;}
     | BIN_VAL { $$ = $1;}
     ;
NEWLINETYPE : EMPTYLINE {
            if(pass == 2)
            fprintf(lst_file,"%d\n",yyline);
        }
            | NEWLINE {newline();}
            ;
%%



void yyerror(const char *s) {
    fprintf(stderr, "Error: %s at %d\n", s,yyline);
    exit(EXIT_FAILURE);
}

int main(int argc,char *argv[]){
    if (argc < 2){
        printf("Error : File not provided!\n");
        return 0;
    }

    // opening input code file
    yyin = fopen(argv[1],"r");
    if(!yyin){
        perror("Error Opening File\n");
        return 0;
    }

    // initialize all headers 
    data_header = (SectionHeader *)malloc(sizeof(SectionHeader));
    text_header = (SectionHeader *)malloc(sizeof(SectionHeader));
    symbol_table_header = (SectionHeader *)malloc(sizeof(SectionHeader));
    strcpy(data_header->section_name,"data");
    strcpy(text_header->section_name,"text");
    strcpy(symbol_table_header->section_name,"symbol table");
    data_header -> size = 0;
    text_header -> size = 0;
    symbol_table_header -> size = 0;

    // opening files
    obj_file = fopen("obj.o","w");
    lst_file = fopen("out.lst","w");
    txt = fopen(".text","w");
    data_file = fopen(".data","w");
    symbt_file = fopen(".symbt","w");

    ensure_file_opened(obj_file,"obj.o");
    ensure_file_opened(lst_file,"out.lst");
    ensure_file_opened(txt,".text");
    ensure_file_opened(symbt_file,".symbt");

    // pass 1
    symbol_table = init_symbol_table();
    pass = 1;
    yyparse();
    display_symbol_table(symbol_table);
    fclose(yyin);

    // pass 2 
    pass = 2;
    yyline = 0;
    yyin = fopen(argv[1],"r");
    if(!yyin){
        perror("Error Opening File\n");
        return 0;
    }
    yyparse();
    
    // building the object file
    symbol_table_header -> size = extract_symbols_in_file(symbol_table,symbt_file);
    unsigned int magic = 0XCAFEBABE;

    fwrite(&magic,sizeof(magic),1,obj_file);
    
    text_header->offset = sizeof(magic) + (3*sizeof(SectionHeader));
    data_header->offset = text_header->offset + text_header->size;
    symbol_table_header->offset = data_header->offset + data_header->size;

    fwrite(text_header,sizeof(SectionHeader),1,obj_file);
    fwrite(data_header,sizeof(SectionHeader),1,obj_file);
    fwrite(symbol_table_header,sizeof(SectionHeader),1,obj_file);

    fclose(txt);    txt = fopen(".text","r");
    fclose(data_file); data_file = fopen(".data","r");   
    fclose(symbt_file); symbt_file = fopen(".symbt","r");

    int bytes_written;

    int buffer_size = 512;
    unsigned char *buffer = (unsigned char *)malloc(buffer_size);

    while ((bytes_written = fread(buffer, 1, buffer_size, txt)) > 0){
        fwrite(buffer,1,bytes_written,obj_file);
    }

    bytes_written = 0;
    while ((bytes_written = fread(buffer, 1, buffer_size, data_file)) > 0){
        fwrite(buffer,1,bytes_written,obj_file);
    }

    bytes_written = 0;
    while ((bytes_written = fread(buffer, 1, buffer_size, symbt_file)) > 0){
        fwrite(buffer,1,bytes_written,obj_file);
    }

    fclose(txt);
    fclose(data_file);
    fclose(symbt_file);
    fclose(yyin);
    fclose(obj_file);

    remove(".text");
    remove(".data");
    remove(".symbt");

    if(argc > 2){
        if (strcmp(argv[3],"-l") != 0) {
            remove("out.lst");
        }
    }
}