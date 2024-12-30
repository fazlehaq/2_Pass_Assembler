#include<stdio.h>
#include<string.h>
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


int binary_str_to_int(char *binary_str){
    binary_str += 2;
    int result = 0;
    while (*binary_str) {
        result = (result << 1) + (*binary_str - '0'); // Left shift and add bit
        binary_str++;
    }
    return result;
}

int hex_str_to_int(char *hex_str){
    return strtol(hex_str, NULL, 16);
}

