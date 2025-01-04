#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"../headers/utils.h"


int get_size_in_bytes(char bytes){
    switch (bytes)
    {
    case 'b' :
        return 1;
    
    case 'w' :
        return 2;

    case 'd' :
        return 4;
    
    case 'q' :
        return 8;
    
    default:
        fprintf(stderr,"Invalid character %c\n",bytes);
        return -1;
        break;
    }
}

int check_number_size(long number, int size) {
    long min, max;

    switch (size) {
        case 1: // 1 byte: range -128 to 127
            min = -128;
            max = 127;
            break;

        case 2: // 2 bytes: range -32,768 to 32,767
            min = -32768;
            max = 32767;
            break;

        case 3: // 3 bytes: range -8,388,608 to 8,388,607
            min = -8388608;
            max = 8388607;
            break;

        case 4: // 4 bytes: range -2,147,483,648 to 2,147,483,647
            min = -2147483648L;
            max = 2147483647L;
            break;

        default: // Invalid size
            return 0;
    }

    // Check if the number is within the valid range
    return (number >= min && number <= max);
}

int get_number_size(int number) {
    if (number <= 127 && number >= -128){
        return 1;
    }

    if(number <= 32767 && number >= -32768){
        return 2;
    }

    if(number <= 2147483647L && number >= -2147483648L){
        return 4;
    }

    printf("Error : Number greater than 32 bits");
}


int parsenum(char *num_str){
    if (strncmp(num_str, "0x", 2) == 0 || strncmp(num_str,"0X",2) == 0) {
        return strtol(num_str, NULL, 16); // Hexadecimal
    } else if (strncmp(num_str, "0b", 2) == 0 || strncmp(num_str, "0B", 2) == 0) {
        return strtol(num_str + 2, NULL, 2); // Binary
    } else {
        return atoi(num_str); // Decimal
    }
}


int strip_extra_chars(char *str) {
    int len = 0;
    while (*str != '\0') { 
        if (*str == ' ' || *str == ':') { 
            break;
        }
        str++;
        len++;
    }
    return len;
}


int get_register_number(char *reg){
    if (strcmp(reg,"eax") == 0)
        return 0;
    if (strcmp(reg,"ecx") == 0)
        return 1;
    if (strcmp(reg,"edx") == 0)
        return 2;
    if (strcmp(reg,"ebx") == 0)
        return 3;
    if (strcmp(reg,"esp") == 0)
        return 4;
    if (strcmp(reg,"ebp") == 0)
        return 5;
    if(strcmp(reg,"esi") == 0)
        return 6;
    if (strcmp(reg,"edi") == 0)
        return 7;

    printf("Error : Invalid input \n");
    return -1;
}

char* get_register_encoding(char *reg){
    if (strcmp(reg,"eax") == 0)
        return "000";
    if (strcmp(reg,"ecx") == 0)
        return "001";
    if (strcmp(reg,"edx") == 0)
        return "010";
    if (strcmp(reg,"ebx") == 0)
        return "011";
    if (strcmp(reg,"esp") == 0)
        return "100";
    if (strcmp(reg,"ebp") == 0)
        return "101";
    if(strcmp(reg,"esi") == 0)
        return "110";
    if (strcmp(reg,"edi") == 0)
        return "111";
}

unsigned char make_mod_rm_byte(char* mod,char* reg,char* rm){
    char mod_byte_str[12];
    snprintf(mod_byte_str,sizeof(mod_byte_str),"0b%s%s%s",mod,reg,rm);
    return (unsigned char) parsenum(mod_byte_str);
}

void to_little_endian(char *buffer,int value,int size){
    for (int i=0 ; i<size; i++){
        sprintf( buffer+(i*2),"%02X",((value >> (8*i)) & 0XFF));
    }
}

void to_lowercase(char *ip,char *op){
    if (ip == NULL) return;
    int i;
    for ( i = 0; ip[i] != '\0'; i++) {
        if (ip[i] >= 'A' && ip[i] <= 'Z') { // Check if the character is uppercase
            op[i] = ip[i] + ('a' - 'A');
        }
    }
    op[i] = '\0';
}

char * to_lowercase_in_place(char *str) {
    if (str == NULL) return NULL;

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') { // Check if the character is uppercase
            str[i] = str[i] + ('a' - 'A');
        }
    }

    return str;
}