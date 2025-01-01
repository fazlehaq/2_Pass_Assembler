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
    switch (size) {
        case 1:
            if ((number & ~0xFF) != 0) // Check if bits outside 1 byte are set
                return 0;
            return 1;

        case 2:
            if ((number & ~0xFFFF) != 0) // Check if bits outside 2 bytes are set
                return 0;
            return 1;

        case 3:
            if ((number & ~0xFFFFFF) != 0) // Check if bits outside 3 bytes are set
                return 0;
            return 1;

        case 4:
            return 1; // Any number fits in 4 bytes
        
        default:
            return 0; // Invalid size
    }
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
