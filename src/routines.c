#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/routines.h"
#include "../headers/symbol-table.h"
#include "../headers/utils.h"

int handle_variable_symbol(int pass, SymbolTable *symbol_table, FILE *lst_file, char *symbol_name, int address, int section, int size, int status, int value)
{
    if (pass == 1)
    {

        int res = insert_symbol(symbol_table, symbol_name, address, section, size, status, value);

        if (res == 0)
        {
            exit(EXIT_FAILURE);
        }
        return 1;
    }
    else
    {
        char *encoding;

        if (section == DATA_SECTION)
        {
            encoding = (char *)malloc((2 * size) + 1);
            to_little_endian(encoding, value, size);
        }
        else if (section == BSS_SECTION)
        {
            if (size <= 8)
            {
                for (int i = 0; i < size; i++)
                    fprintf(lst_file, "??");
            }
            else
            {
                fprintf(lst_file, "<res %Xh>", size);
            }
        }
    }
    return 1;
}

void handle_label(int pass, SymbolTable *symbol_table, char *symbol_name, int address, int section, int status)
{
    // fecth the symbol
    Symbol *symbol = search_symbol(symbol_table, symbol_name);
    if (pass == 1)
    {

        if (symbol && symbol->section == TEXT_SECTION)
        {
            // Already defined
            if (symbol->status == DEFINED_SYMBOL)
            {
                // Redifining
                if (status == DEFINED_SYMBOL)
                {
                    printf("Error : Redifining Symbol : %s\n", symbol_name);
                    exit(EXIT_FAILURE);
                }
            }
            // symbol present is undefined
            else if (symbol->status == UNDEFINED_SYMBOL)
            {
                // Do nothing
                if (status == UNDEFINED_SYMBOL)
                {
                }
                // update the symbol
                else if (status == DEFINED_SYMBOL)
                {
                    symbol->address = address;
                    symbol->status = DEFINED_SYMBOL;
                }
            }
        }
        else if (!symbol && section == TEXT_SECTION)
        {
            insert_symbol(symbol_table, symbol_name, address, section, 0, status, -1);
        }
        else
        {
            printf("Error : Invalid use of symbol %s\n", symbol_name);
            exit(EXIT_FAILURE);
        }
    }
}

// checks if inst is valid and returns the size of inst
int handle_op_label(int pass, SymbolTable *symbol_table, FILE *lst_file, int curr_address, char *op_name, char *label_name)
{
    int inst_size;

    if (
        strcmp(op_name, "jmp") == 0 ||
        strcmp(op_name, "jz") == 0 ||
        strcmp(op_name, "jnz") == 0)
    {
        // assumption is that jmps will be relative 8 bits only
        inst_size = 2;
    }
    else
    {
        printf("Error : Invalid use of label : %s\n", label_name);
        exit(EXIT_FAILURE);
    }

    if (pass == 2)
    {
        char *encoding = (char *)malloc(2 + 1 + 8 + 1 + 1);
        Symbol *symbol = search_symbol(symbol_table, label_name);
        unsigned char opcode;

        if (strcmp(op_name, "jz") == 0)
            opcode = 0x74;
        else if (strcmp(op_name, "jnz") == 0)
            opcode = 0x75;
        else if (strcmp(op_name, "jmp") == 0)
            opcode = 0xEB;

        char displcement = (char)((symbol->address) - (curr_address + 2));
        snprintf(encoding, 2 + 1 + 8 + 1 + 1, "%02X%02X", (unsigned char)opcode, (unsigned char)displcement);
        fprintf(lst_file, "%s", encoding);
        free(encoding);
    }

    return inst_size;
}

int handle_op_register(int pass, FILE *lst_file, char *op_name, char *reg)
{
    // Div Mul-> 2 bytes
    // INC DEC -> 1 bytes
    int inst_size;

    if (strcmp(op_name, "div") == 0 || strcmp(op_name, "mul") == 0)
        inst_size = 2;
    else if (strcmp(op_name, "inc") == 0 || strcmp(op_name, "dec") == 0)
        inst_size = 1;
    else
    {
        printf("Error : Invalid Instruction : %s\n", op_name);
        exit(EXIT_FAILURE);
    }

    if (pass == 2)
    {
        char *encoding = NULL;

        if (strcmp(op_name, "inc") == 0)
        {
            encoding = (char *)malloc(3);
            snprintf(encoding, 3, "%02X", 0x40 + get_register_number(reg));
        }
        else if (strcmp(op_name, "dec") == 0)
        {
            encoding = (char *)malloc(3);
            snprintf(encoding, 3, "%02X", 0x48 + get_register_number(reg));
        }
        else if (strcmp(op_name, "mul") == 0)
        {
            encoding = (char *)malloc(5);
            char *mod_bits = "11";
            char *reg_bits = "100";
            char *rm_bits = get_register_encoding(reg);
            unsigned char mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
            snprintf(encoding, 5, "%02X%02X", 0xF7, mod_byte);
        }
        else if (strcmp(op_name, "div") == 0)
        {
            encoding = (char *)malloc(5);
            char *mod_bits = "11";
            char *reg_bits = "110";
            char *rm_bits = get_register_encoding(reg);
            unsigned char mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
            snprintf(encoding, 5, "%02X%02X", 0xF7, mod_byte);
        }

        else
        {
            printf("Unsupported instruction: %s\n", op_name);
            exit(EXIT_FAILURE);
        }

        fprintf(lst_file, "%s", encoding);
        free(encoding);
    }
    return inst_size;
}

int handle_op_reg_reg(int pass, FILE *lst_file, char *op_name, char *reg1, char *reg2)
{
    int inst_size;

    if (
        strcmp(op_name, "mov") == 0 ||
        strcmp(op_name, "add") == 0 ||
        strcmp(op_name, "sub") == 0 ||
        strcmp(op_name, "xor") == 0 ||
        strcmp(op_name, "cmp") == 0)
        inst_size = 2;
    else
    {
        printf("Error : instruction %s doesnot support register register operation\n", op_name);
        exit(EXIT_FAILURE);
    }

    if (pass == 2)
    {
        char *encoding = (char *)malloc(5);
        unsigned char opcode;
        char *mod_bits = "11";
        char *reg_bits = get_register_encoding(reg2);
        char *rm_bits = get_register_encoding(reg1);

        if (strcmp(op_name, "mov") == 0)
            opcode = 0x89;
        else if (strcmp(op_name, "add") == 0)
            opcode = 0X01;
        else if (strcmp(op_name, "sub") == 0)
            opcode = 0X29;
        else if (strcmp(op_name, "cmp") == 0)
            opcode = 0X39;
        else if (strcmp(op_name, "xor") == 0)
            opcode = 0X31;

        unsigned char mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
        snprintf(encoding, 5, "%02X%02X", opcode, mod_byte);
        fprintf(lst_file, "%s", encoding);
        free(encoding);
    }

    return inst_size;
}

int handle_reg_addr_to_reg(int pass, FILE *lst_file, char *op_name, char *reg1, char *reg2)
{
    int inst_size;

    if (
        strcmp("add", op_name) == 0 ||
        strcmp("mov", op_name) == 0 ||
        strcmp("sub", op_name) == 0 ||
        strcmp("xor", op_name) == 0 ||
        strcmp("cmp", op_name) == 0)
        inst_size = 2;
    else
    {
        printf("Instruction %s doesnot support r/m32 , r32 operation\n", op_name);
        exit(EXIT_FAILURE);
    }

    if (pass == 2)
    {
        char *encoding = (char *)malloc(5);
        unsigned char opcode;
        char *mod_bits = "00";
        char *reg_bits = get_register_encoding(reg2);
        char *rm_bits = get_register_encoding(reg1);

        if (strcmp(op_name, "mov") == 0)
            opcode = 0x89;
        else if (strcmp(op_name, "add") == 0)
            opcode = 0X01;
        else if (strcmp(op_name, "sub") == 0)
            opcode = 0X29;
        else if (strcmp(op_name, "cmp") == 0)
            opcode = 0X39;
        else if (strcmp(op_name, "xor") == 0)
            opcode = 0X31;

        unsigned char mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
        snprintf(encoding, 5, "%02X%02X", opcode, mod_byte);
        fprintf(lst_file, "%s", encoding);
        free(encoding);
    }
    return inst_size;
}

int handle_reg_to_immd_val(int pass, FILE *lst_file, char *op_name, char *reg1, int value)
{

    int inst_size;
    int byte_size = get_number_size(value);
        // for any imm value mov B8 +rd id

        // eax
        // add -> 1B -> 83 Modrm ib | -> 2B -> 05 id
        // sub -> 1B -> 83 Modrm ib | -> 2B -> 2D id
        // cmp -> 1b -> 83 Modrm ib | -> 2B -> 3D id
        // xor -> 1B -> 83 modrm ib | -> 2B -> 35 id

        // any other reg
        // add -> 1B -> 83 Modrm ib | -> 2B ->  81 modrm id
        // sub -> 1B -> 83 Modrm ib | -> 2B -> 81 modrm id
        // cmp -> 1B -> 83 Modrm ib | -> 2B -> 81 modrm id
        // xor -> 1B -> 83 Modrm ib | -> 2B -> 81 modrm id 
    
    if (strcmp("mov",op_name) == 0)
        inst_size = 5;
    else if(byte_size == 1)
        inst_size = 3;
    else if (strcmp("eax",reg1) ==0 && byte_size > 1)
        inst_size = 5;
    else if(strcmp("eax",reg1)!=0) 
        inst_size = 6;


    else
    {
        printf("Error : Operation %s does not support reg to immediate value!\n", op_name);
        exit(EXIT_FAILURE);
    }
    if (pass == 2)
    {
        char *encoding;
        unsigned char opcode;
        int byte_size = get_number_size(value);

        // for any imm value mov B8 +rd id

        // eax
        // add -> 1B -> 83 Modrm ib | -> 2B -> 05 id
        // sub -> 1B -> 83 Modrm ib | -> 2B -> 2D id
        // cmp -> 1b -> 83 Modrm ib | -> 2B -> 3D id
        // xor -> 1B -> 83 modrm ib | -> 2B -> 35 id

        // any other reg
        // add -> 1B -> 83 Modrm ib | -> 2B ->  81 modrm id
        // sub -> 1B -> 83 Modrm ib | -> 2B -> 81 modrm id
        // cmp -> 1B -> 83 Modrm ib | -> 2B -> 81 modrm id
        // xor -> 1B -> 83 Modrm ib | -> 2B -> 81 modrm id

        if (strcmp(op_name, "mov") == 0)
        {
            opcode = (unsigned char)(0XB8 + get_register_number(reg1));
            encoding = (char *)malloc(2 + 8 + 1);

            snprintf(encoding, 3, "%02X", opcode);
            to_little_endian(encoding + 2, value, 4);
        }
        else
        {
            char *mod_bits = "11";
            char *reg_bits;
            char *rm_bits = get_register_encoding(reg1);
            if (byte_size == 1)
            {
                opcode = 0X83;
                encoding = (char *)malloc(2 + 2 + (2 * 1) + 1);
                if (strcmp(op_name, "add") == 0)
                    reg_bits = "000";

                else if (strcmp(op_name, "sub") == 0)
                    reg_bits = "101";

                else if (strcmp(op_name, "xor") == 0)
                    reg_bits = "110";

                else if (strcmp(op_name, "cmp") == 0)
                    reg_bits = "111";

                unsigned char mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
                snprintf(encoding, 5, "%02X%02X", opcode, mod_byte);
                to_little_endian(encoding + 4, value, 1);
            }
            else if (byte_size == 2 || byte_size == 4)
            {
                if (strcmp(reg1, "eax") != 0)
                {
                    encoding = (char *)malloc(2 + 2 + (2 * 4) + 1);
                    opcode = 0X81;
                    if (strcmp(op_name, "add") == 0)
                    {
                        reg_bits = "000";
                    }
                    else if (strcmp(op_name, "sub") == 0)
                    {
                        reg_bits = "101";
                    }
                    else if (strcmp(op_name, "xor") == 0)
                    {
                        reg_bits = "110";
                    }
                    else if (strcmp(op_name, "cmp") == 0)
                    {
                        reg_bits = "111";
                    }

                    unsigned char mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
                    snprintf(encoding, 5, "%02X%02X", opcode, mod_byte);
                    to_little_endian(encoding + 4, value, 4);
                }
                else
                {
                    encoding = (char *)malloc(2 + (2 * 4) + 1);

                    if (strcmp(op_name, "add") == 0)
                    {
                        opcode = 0X05;
                    }
                    else if (strcmp(op_name, "sub") == 0)
                    {
                        opcode = 0X2D;
                    }
                    else if (strcmp(op_name, "xor") == 0)
                    {
                        opcode = 0X35;
                    }
                    else if (strcmp(op_name, "cmp") == 0)
                    {
                        opcode = 0X3D;
                    }

                    snprintf(encoding, 2 + 1, "%02X", opcode);
                    to_little_endian(encoding + 2, value, 4);
                }
            }
        }

        fprintf(lst_file, "%s", encoding);
        free(encoding);
    }
    return inst_size;
}

int handle_reg_to_label(int pass, SymbolTable *symbol_table, FILE *lst_file, char *op_name, char *reg1, char *label_name)
{
    Symbol *symbol = search_symbol(symbol_table, label_name);

    int inst_size; // special base case for eax
        if (strcmp(reg1, "eax") == 0)
        {
            if (
                strcmp(op_name, "add") == 0 ||
                strcmp(op_name, "sub") == 0 ||
                strcmp(op_name, "xor") == 0 ||
                strcmp(op_name, "cmp") == 0)
                inst_size =  5;
        }
        // mov -> 5 bytes , add -> 6 , sub -> 6 , xor -> 6 , cmp -> 6
        else if (strcmp("mov", op_name) == 0)
            inst_size = 5;

        else if (
            strcmp("add", op_name) == 0 ||
            strcmp("sub", op_name) == 0 ||
            strcmp("xor", op_name) == 0 ||
            strcmp("cmp", op_name) == 0)
            inst_size =  6;

        else
        {
            printf("Error : Operation %s does not support reg to immediate value!\n", op_name);
            exit(EXIT_FAILURE);
        }
    
    if (pass == 2)
    {
        // mov -> B8 +rd id
        // eax
        // add -> 05 id
        // sub -> 2D id
        // xor -> 35 id
        // cmp -> 3D id
        // other
        // 81 /0 id
        // 81 /5 id
        // 81 /6 id
        // 81 /7 id
        unsigned char opcode;
        char *encoding;

        if (strcmp(op_name, "mov") == 0)
        {
            opcode = (unsigned char)(0XB8 + get_register_number(reg1));
            encoding = (char *)malloc(2 + 2 + 8 + 2 + 1);

            snprintf(encoding, 2 + 1 + 1, "%02X[", opcode);
            to_little_endian(encoding + 3, symbol->address, 4);
            snprintf(encoding + 3 + 8, 2, "%s", "]");
        }
        else
        {
            if (strcmp(reg1, "eax") != 0)
            {
                char *reg_bits;
                char *rm_bits = get_register_encoding(reg1);
                char *mod_bits = "11";

                encoding = (char *)malloc(2 + 2 + (2 * 4) + 1);
                opcode = 0X81;
                if (strcmp(op_name, "add") == 0)
                    reg_bits = "000";

                else if (strcmp(op_name, "sub") == 0)
                    reg_bits = "101";

                else if (strcmp(op_name, "xor") == 0)
                    reg_bits = "110";

                else if (strcmp(op_name, "cmp") == 0)
                    reg_bits = "111";

                unsigned char mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
                snprintf(encoding, 6, "%02X%02X[", opcode, mod_byte);
                to_little_endian(encoding + 5, symbol->address, 4);
                snprintf(encoding + 5 + 8, 2, "]");
            }
            else
            {
                encoding = (char *)malloc(2 + 1 + (2 * 4) + 1 + 1);

                if (strcmp(op_name, "add") == 0)
                    opcode = 0X05;
                else if (strcmp(op_name, "sub") == 0)
                    opcode = 0X2D;
                else if (strcmp(op_name, "xor") == 0)
                    opcode = 0X35;
                else if (strcmp(op_name, "cmp") == 0)
                    opcode = 0X3D;

                snprintf(encoding, 2 + 1 + 1, "%02X[", opcode);
                to_little_endian(encoding + 3, symbol->address, 4);
                snprintf(encoding + 3 + 8, 2, "]");
            }
        }

        fprintf(lst_file, "%s", encoding);
        free(encoding);
    }
    return inst_size;
}

int handle_reg_to_label_address(int pass, SymbolTable *symbol_table, FILE *lst_file, char *op_name, char *reg1, char *label_name)
{
    Symbol *symbol = search_symbol(symbol_table, label_name);

    int inst_size;
        if (strcmp(reg1, "eax") == 0 && strcmp(op_name, "mov") == 0)
            inst_size =  5;

        if (
            strcmp(op_name, "mov") == 0 ||
            strcmp(op_name, "add") == 0 ||
            strcmp(op_name, "sub") == 0 ||
            strcmp(op_name, "xor") == 0 ||
            strcmp(op_name, "cmp") == 0)
            inst_size = 6;

        else
        {
            printf("Error : Invalid Instruction: \n");
            exit(EXIT_FAILURE);
        }
    

    if (pass == 2)
    {
        char *encoding;
        // mov eax [label] -> A1[addrr]
        // mov 8B /r
        // add 03 /r
        // sub 2B /r
        // xor 33 /r
        // cmp 3B /r

        if (strcmp("eax", reg1) == 0 && strcmp(op_name, "mov") == 0)
        {
            encoding = (char *)malloc(2 + 1 + 8 + 1 + 1);
            snprintf(encoding, 2 + 1 + 1, "%02X[", 0XA1);
            to_little_endian(encoding + 3, symbol->address, 4);
            snprintf(encoding + 2 + 1 + 8, 2, "]");
        }
        else
        {
            encoding = (char *)malloc(4 + 1 + 8 + 1 + 1);
            char *reg_bits = get_register_encoding(reg1);
            char *rm_bits = "101"; // special value for disp32
            char *mod_bits = "00"; // memory mode with no displacements
            unsigned char mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
            unsigned char opcode;

            if (strcmp(op_name, "mov") == 0)
                opcode = 0X8B;
            if (strcmp(op_name, "add") == 0)
                opcode = 0X03;
            if (strcmp(op_name, "sub") == 0)
                opcode = 0X2B;
            if (strcmp(op_name, "xor") == 0)
                opcode = 0X33;
            if (strcmp(op_name, "cmp") == 0)
                opcode = 0X3B;

            snprintf(encoding, 4 + 1 + 1, "%02X%02X[", opcode, mod_byte);
            to_little_endian(encoding + 5, symbol->address, 4);
            snprintf(encoding + 5 + 8, 2, "]");
        }

        fprintf(lst_file, "%s", encoding);
        free(encoding);
    }

    return inst_size;
}

int handle_reg_to_immd_address(int pass, FILE *lst_file, char *op_name, char *reg1, int value)
{
    if (!check_number_size((long)value, 4))
    {
        printf("Error : Exceeding limit :\n");
        exit(EXIT_FAILURE);
    }

    int inst_size ;
        if (strcmp(op_name, "mov") == 0)
            inst_size =  5;
        if (
            strcmp(op_name, "add") == 0 ||
            strcmp(op_name, "sub") == 0 ||
            strcmp(op_name, "xor") == 0 ||
            strcmp(op_name, "cmp") == 0)
            inst_size =  6;
    
    if (pass == 2)
    {
        char *encoding;
        // mov eax [label] -> A1[addrr]
        // mov 8B /r
        // add 03 /r
        // sub 2B /r
        // xor 33 /r
        // cmp 3B /r

        if (strcmp("eax", reg1) == 0 && strcmp(op_name, "mov") == 0)
        {
            encoding = (char *)malloc(2 + 8 + 1);
            snprintf(encoding, 2 + 1, "%02X", 0XA1);
            to_little_endian(encoding + 2, value, 4);
        }
        else
        {
            encoding = (char *)malloc(4 + 8 + 1);
            char *reg_bits = get_register_encoding(reg1);
            char *rm_bits = "101"; // special value for disp32
            char *mod_bits = "00"; // memory mode with no displacements
            unsigned char mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
            unsigned char opcode;

            if (strcmp(op_name, "mov") == 0)
                opcode = 0X8B;
            if (strcmp(op_name, "add") == 0)
                opcode = 0X03;
            if (strcmp(op_name, "sub") == 0)
                opcode = 0X2B;
            if (strcmp(op_name, "xor") == 0)
                opcode = 0X33;
            if (strcmp(op_name, "cmp") == 0)
                opcode = 0X3B;

            snprintf(encoding, 4 + 1, "%02X%02X", opcode, mod_byte);
            to_little_endian(encoding + 4, value, 4);
        }

        fprintf(lst_file, "%s", encoding);
        free(encoding);
    }

    return inst_size;
}

int handle_reg_to_reg_address(int pass, FILE *lst_file, char *op_name, char *reg1, char *reg2)
{
    int inst_size ;
    if (
            strcmp(op_name, "mov") ||
            strcmp(op_name, "add") ||
            strcmp(op_name, "sub") ||
            strcmp(op_name, "xor") ||
            strcmp(op_name, "cmp"))
            inst_size =  2;

        else
        {
            printf("Error : %s doesnot support r to r/m32 operation\n", op_name);
            exit(EXIT_FAILURE);
        }
    
     if (pass == 2)
    {
        char *encoding = (char *)malloc(5);
        unsigned char opcode;
        char *mod_bits = "00";
        char *reg_bits = get_register_encoding(reg1);
        char *rm_bits = get_register_encoding(reg2);

        if (strcmp(op_name, "mov") == 0)
            opcode = 0x8B;
        else if (strcmp(op_name, "add") == 0)
            opcode = 0X03;
        else if (strcmp(op_name, "sub") == 0)
            opcode = 0X2B;
        else if (strcmp(op_name, "cmp") == 0)
            opcode = 0X3B;
        else if (strcmp(op_name, "xor") == 0)
            opcode = 0X33;

        unsigned char mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
        snprintf(encoding, 5, "%02X%02X", opcode, mod_byte);
        fprintf(lst_file, "%s", encoding);
        free(encoding);
    }
    return inst_size ;
}

int handle_op_reg_addr(int pass, FILE *lst_file, char *op_name, char *reg1)
{
    int inst_size;
        if (
            strcmp("inc", op_name) == 0 ||
            strcmp("dec", op_name) == 0 ||
            strcmp("mul", op_name) == 0 ||
            strcmp("div", op_name) == 0)
            inst_size= 2;

        else
        {
            printf("Error : %s doesnot support reg-memory operation\n!", op_name);
            exit(EXIT_FAILURE);
        }
     if (pass == 2)
    {
        // mod -> 00
        // r/m -> reg1
        // inc -> FF  /0
        // dec -> FF  /1
        // mul -> F7  /4
        // div -> F7  /6
        char *encoding = (char *)malloc(2 + 2 + 1);
        char *mod_bits = "00";
        char *rm_bits = get_register_encoding(reg1);
        char *reg_bits;
        unsigned char opcode;
        unsigned char mod_byte;

        if (strcmp(op_name, "inc") == 0)
        {
            opcode = 0XFF;
            reg_bits = "000";
        }
        if (strcmp(op_name, "dec") == 0)
        {
            opcode = 0XFF;
            reg_bits = "001";
        }
        if (strcmp(op_name, "mul") == 0)
        {
            opcode = 0XF7;
            reg_bits = "100";
        }
        if (strcmp(op_name, "div") == 0)
        {
            opcode = 0XF7;
            reg_bits = "110";
        }

        mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
        snprintf(encoding, 5, "%02X%02X", opcode, mod_byte);

        fprintf(lst_file, "%s", encoding);
        free(encoding);
    }
    return inst_size;
}

int handle_dword_reg_addr_to_immd(int pass, FILE *lst_file, char *op_name, char *reg1, int value)
{
    int inst_size;
        if (
            strcmp(op_name, "mov") ||
            strcmp(op_name, "add") ||
            strcmp(op_name, "sub") ||
            strcmp(op_name, "xor") ||
            strcmp(op_name, "cmp"))
            inst_size =  6;
        else
        {
            printf("Error : %s doesnot support r/m32 to immd operation\n", op_name);
            exit(EXIT_FAILURE);
        }
    
     if (pass == 2)
    {
        // mod = 00
        // rm <- reg1
        // mov -> c7 /0
        // add -> 81 /0
        // sub -> 81 /5
        // xor -> 81 /6
        // cmp -> 81 /7

        char *mod_bits = "00";
        char *rm_bits = get_register_encoding(reg1);
        char *reg_bits;
        unsigned char mod_byte;
        unsigned char opcode;
        if (strcmp(op_name, "mov") == 0)
        {
            opcode = 0XC7;
            reg_bits = "000";
        }
        else
        {
            opcode = 0X81;
            if (strcmp(op_name, "add") == 0)
                reg_bits = "000";
            if (strcmp(op_name, "sub") == 0)
                reg_bits = "101";
            if (strcmp(op_name, "xor") == 0)
                reg_bits = "110";
            if (strcmp(op_name, "cmp") == 0)
                reg_bits = "111";
        }
        char *encoding = malloc(13);
        mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
        snprintf(encoding, 5, "%02X%02X", opcode, mod_byte);
        to_little_endian(encoding + 4, value, 4);

        fprintf(lst_file, "%s", encoding);
        free(encoding);
    }
    return inst_size;
}
