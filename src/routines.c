#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/routines.h"
#include "../headers/symbol-table.h"
#include "../headers/utils.h"

int handle_variable_symbol(int pass, SymbolTable *symbol_table, char *symbol_name, int address, int section, int size, int status, int value)
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
        printf("Error : Pass 2 not implemented yet!\n");
        exit(EXIT_FAILURE);
    }
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
    else if (pass == 2)
    {
        printf("Not implemented yet!\n");
        exit(EXIT_FAILURE);
    }
}

// checks if inst is valid and returns the size of inst
int handle_op_label(int pass, char *op_name, char *label_name)
{
    if (
        strcmp(op_name, "jmp") == 0 ||
        strcmp(op_name, "jz") == 0 ||
        strcmp(op_name, "jnz") == 0 ||
        strcmp(op_name, "JMP") == 0 ||
        strcmp(op_name, "JZ") == 0 ||
        strcmp(op_name, "JNZ") == 0)
    {
        // assumption is that jmps will be relative 8 bits only
        return 2;
    }
    printf("Error : Invalid use of label : %s\n", label_name);
    exit(EXIT_FAILURE);
}

int handle_op_register(int pass, char *op_name, char *reg)
{
    // Div Mul-> 2 bytes
    // INC DEC -> 1 bytes
    if (pass == 1)
    {
        if (
            strcmp(op_name, "DIV") == 0 ||
            strcmp(op_name, "div") == 0 ||
            strcmp(op_name, "MUL") == 0 ||
            strcmp(op_name, "mul") == 0)
        {
            return 2;
        }
        else if (
            strcmp(op_name, "INC") == 0 ||
            strcmp(op_name, "inc") == 0 ||
            strcmp(op_name, "DEC") == 0 ||
            strcmp(op_name, "dec") == 0)
        {
            return 1;
        }
        else
        {
            printf("Error : Invalid Instruction : %s\n", op_name);
            exit(EXIT_FAILURE);
        }
    }
    else if (pass == 2)
    {
        char *encoding = NULL;

        if (strcmp(op_name, "INC") == 0 || strcmp(op_name, "inc") == 0)
        {
            encoding = (char *)malloc(3); 
            snprintf(encoding, 3, "%02X", 0x40 + get_register_number(reg));
        }
        else if (strcmp(op_name, "DEC") == 0 || strcmp(op_name, "dec") == 0)
        {
            encoding = (char *)malloc(3); 
            snprintf(encoding, 3, "%02X", 0x48 + get_register_number(reg));
        }
        else if (strcmp(op_name, "MUL") == 0 || strcmp(op_name, "mul") == 0) {
            encoding = (char *)malloc(5); 
            char *mod_bits = "11";
            char *reg_bits = "100";
            char *rm_bits = get_register_encoding(reg); 
            unsigned char mod_byte = make_mod_rm_byte(mod_bits, reg_bits, rm_bits);
            snprintf(encoding, 5, "%02X%02X", 0xF7, mod_byte);
        }
        else if (strcmp(op_name, "DIV") == 0 || strcmp(op_name, "div") == 0)
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
            return;
        }

        // Output the encoding
        printf("Encoding: %s\n", encoding);

        // Free allocated memory
        free(encoding);
    }
    return 0;
}

int handle_op_reg_reg(int pass, char *op_name, char *reg1, char *reg2)
{
    if (pass == 1)
    {
        if (
            strcmp(op_name, "MOV") == 0 ||
            strcmp(op_name, "mov") == 0 ||
            strcmp(op_name, "add") == 0 ||
            strcmp(op_name, "ADD") == 0 ||
            strcmp(op_name, "sub") == 0 ||
            strcmp(op_name, "SUB") == 0 ||
            strcmp(op_name, "xor") == 0 ||
            strcmp(op_name, "XOR") == 0 ||
            strcmp(op_name, "CMP") == 0 ||
            strcmp(op_name, "cmp") == 0)
        {
            return 2;
        }
        else
        {
            printf("Error : instruction %s doesnot support register register operation\n", op_name);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf("Error : Pass 2 not implemented yet!\n");
        exit(EXIT_FAILURE);
    }
}

int handle_reg_addr_to_reg(int pass, char *op_name, char *reg1, char *reg2)
{
    if (pass == 1)
    {

        if (
            strcmp("ADD", op_name) == 0 ||
            strcmp("add", op_name) == 0 ||
            strcmp("MOV", op_name) == 0 ||
            strcmp("mov", op_name) == 0 ||
            strcmp("SUB", op_name) == 0 ||
            strcmp("sub", op_name) == 0 ||
            strcmp("XOR", op_name) == 0 ||
            strcmp("xor", op_name) == 0 ||
            strcmp("cmp", op_name) == 0 ||
            strcmp("CMP", op_name) == 0)
        {
            return 2;
        }
        else
        {
            printf("Instruction %s doesnot support r/m32 , r32 operation\n", op_name);
            exit(EXIT_FAILURE);
        }
    }
    else if (pass == 2)
    {
        printf("Pass 2 not implemented yet\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int handle_reg_to_immd_val(int pass, char *op_name, char *reg1, int value)
{
    if (pass == 1)
    { // special base case for eax
        if (strcmp(reg1, "eax") == 0)
        {
            if (
                strcmp(op_name, "add") == 0 ||
                strcmp(op_name, "sub") == 0 ||
                strcmp(op_name, "xor") == 0)
            {
                return 5;
            }
        }
        // mov -> 5 bytes , add -> 6 , sub -> 6 , xor -> 6 , cmp -> 6
        if (
            strcmp("mov", op_name) == 0 ||
            strcmp("MOV", op_name) == 0)
        {
            return 5;
        }
        if (
            strcmp("add", op_name) == 0 ||
            strcmp("ADD", op_name) == 0 ||
            strcmp("sub", op_name) == 0 ||
            strcmp("SUB", op_name) == 0 ||
            strcmp("xor", op_name) == 0 ||
            strcmp("XOR", op_name) == 0 ||
            strcmp("CMP", op_name) == 0 ||
            strcmp("cmp", op_name) == 0)
        {
            return 6;
        }

        else
        {
            printf("Error : Operation %s does not support reg to immediate value!\n", op_name);
            exit(EXIT_FAILURE);
        }
    }
    else if (pass == 2)
    {
        printf("Pass 2 not implemented yet\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int handle_reg_to_label(int pass, char *op_name, char *reg1, char *label_name)
{
    if (pass == 1)
    { // special base case for eax
        if (strcmp(reg1, "eax") == 0)
        {
            if (
                strcmp(op_name, "add") == 0 ||
                strcmp(op_name, "sub") == 0 ||
                strcmp(op_name, "xor") == 0 ||
                strcmp(op_name, "cmp") == 0)
            {
                return 5;
            }
        }
        // mov -> 5 bytes , add -> 6 , sub -> 6 , xor -> 6 , cmp -> 6
        if (
            strcmp("mov", op_name) == 0 ||
            strcmp("MOV", op_name) == 0)
        {
            return 5;
        }
        if (
            strcmp("add", op_name) == 0 ||
            strcmp("ADD", op_name) == 0 ||
            strcmp("sub", op_name) == 0 ||
            strcmp("SUB", op_name) == 0 ||
            strcmp("xor", op_name) == 0 ||
            strcmp("XOR", op_name) == 0 ||
            strcmp("CMP", op_name) == 0 ||
            strcmp("cmp", op_name) == 0)
        {
            return 6;
        }

        else
        {
            printf("Error : Operation %s does not support reg to immediate value!\n", op_name);
            exit(EXIT_FAILURE);
        }
    }
    else if (pass == 2)
    {
        printf("Pass 2 not implemented yet\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int handle_reg_to_label_address(int pass, char *op_name, char *reg1, char *label_name)
{
    if (pass == 1)
    {
        if (strcmp(reg1, "eax") == 0)
        {
            if (strcmp(op_name, "mov") == 0)
            {
                return 5;
            }
        }
        if (
            strcmp(op_name, "mov") || strcmp(op_name, "MOV") ||
            strcmp(op_name, "add") || strcmp(op_name, "ADD") ||
            strcmp(op_name, "sub") || strcmp(op_name, "SUB") ||
            strcmp(op_name, "xor") || strcmp(op_name, "XOR") ||
            strcmp(op_name, "cmp") || strcmp(op_name, "CMP"))
        {
            return 6;
        }
        else
        {
            printf("Error : operator %s does not support the reg to address instruction\n", op_name);
            exit(EXIT_FAILURE);
        }
    }
    else if (pass == 2)
    {
        printf("Error : Pass 2 not implemented yet!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int handle_reg_to_immd_address(int pass, char *op_name, char *reg1, int value)
{
    if (!check_number_size((long)value, 4))
    {
        printf("Error : Exceeding limit :\n");
        exit(EXIT_FAILURE);
    }

    if (pass == 1)
    {

        if (strcmp(op_name, "mov") == 0 || strcmp(op_name, "MOV") == 0)
            return 5;
        if (
            strcmp(op_name, "add") == 0 || strcmp(op_name, "ADD") == 0 ||
            strcmp(op_name, "sub") == 0 || strcmp(op_name, "SUB") == 0 ||
            strcmp(op_name, "xor") == 0 || strcmp(op_name, "XOR") == 0 ||
            strcmp(op_name, "cmp") == 0 || strcmp(op_name, "CMP") == 0)
        {
            return 6;
        }
    }
    else if (pass == 2)
    {
        printf("Error : Pass 2 not implemented yet!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int handle_reg_to_reg_address(int pass, char *op_name, char *reg1, char *reg2)
{
    if (pass == 1)
    {
        if (
            strcmp(op_name, "mov") || strcmp(op_name, "MOV") ||
            strcmp(op_name, "add") || strcmp(op_name, "ADD") ||
            strcmp(op_name, "sub") || strcmp(op_name, "SUB") ||
            strcmp(op_name, "xor") || strcmp(op_name, "XOR") ||
            strcmp(op_name, "cmp") || strcmp(op_name, "CMP"))
        {
            return 2;
        }
        else
        {
            printf("Error : %s doesnot support r to r/m32 operation\n", op_name);
            exit(EXIT_FAILURE);
        }
    }
    else if (pass == 2)
    {
        printf("Error : Pass2 not implemented yet\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int handle_op_reg_addr(int pass, char *op_name, char *reg1)
{
    if (pass == 1)
    {
        if (
            strcmp("inc", op_name) == 0 || strcmp("INC", op_name) == 0 ||
            strcmp("dec", op_name) == 0 || strcmp("DEC", op_name) == 0 ||
            strcmp("mul", op_name) == 0 || strcmp("MUL", op_name) == 0 ||
            strcmp("div", op_name) == 0 || strcmp("DIV", op_name) == 0)
        {
            return 2;
        }
        else
        {
            printf("Error : %s doesnot support reg-memory operation\n!", op_name);
            exit(EXIT_FAILURE);
        }
    }
    else if (pass == 2)
    {
        printf("Error : Pass 2 not implemented yet!\n");
        exit(EXIT_FAILURE);
    }
}

int handle_dword_reg_addr_to_immd(int pass, char *op_name, char *reg1, int value)
{
    if (pass == 1)
    {
        if (
            strcmp(op_name, "mov") || strcmp(op_name, "MOV") ||
            strcmp(op_name, "add") || strcmp(op_name, "ADD") ||
            strcmp(op_name, "sub") || strcmp(op_name, "SUB") ||
            strcmp(op_name, "xor") || strcmp(op_name, "XOR") ||
            strcmp(op_name, "cmp") || strcmp(op_name, "CMP"))
        {
            return 6;
        }
        else
        {
            printf("Error : %s doesnot support r/m32 to immd operation\n", op_name);
            exit(EXIT_FAILURE);
        }
    }
    else if (pass == 2)
    {
        printf("Error : Pass 2 not implemented yet!\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}
