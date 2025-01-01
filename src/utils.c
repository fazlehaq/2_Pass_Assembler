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
