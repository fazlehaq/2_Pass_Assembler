#ifndef OBJ_FILE_H
#define OBJ_FILE_H

typedef struct SectionHeader {
    char section_name[16]; 
    unsigned int offset;  
    unsigned int size;    
} SectionHeader;

typedef struct SymbolEntry {
    char name[16];
    char section;
    char status;
    int size;
    int address;
} SymbolEntry;

#endif 