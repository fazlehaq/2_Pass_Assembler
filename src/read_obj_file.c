#include<stdio.h>
#include<stdlib.h>
#include"../headers/obj_file.h"

void print_section(char *section_name,unsigned int section_offset, unsigned int section_size,FILE *obj_file){
    fseek(obj_file,section_offset,SEEK_SET);
    unsigned char byte; 
    unsigned int cnt=0;
    while(cnt < section_size){
        fread(&byte,1,1,obj_file);
        cnt++;
        printf("%02X ",byte);
        if((cnt % 16) == 0) printf("\n");
    }
    printf("\n");
}

void print_symbol_table(unsigned int size,unsigned int offset, FILE *obj_file){
    SymbolEntry entry;
    printf("Symbol Table:\n");
    printf("%-16s %-8s %-8s %-8s\n", "Name", "Section", "Status", "Address");
    while (fread(&entry, sizeof(SymbolEntry), 1, obj_file) == 1) {
        printf("%-16s %-8d %-8d %-08X\n", entry.name, (int)entry.section, (int)entry.status, entry.address);
    }
}

void read_obj(char *filename){
    FILE *obj_fp = fopen(filename,"r");
    if(!obj_fp) {
        perror("Could not open file\n");
        exit(EXIT_FAILURE);
    }

    unsigned int magic;
    fread(&magic,4,1,obj_fp);
    printf("Magic Number : %08X\n\n",magic);

    int section_cnt = 3;
    SectionHeader *sections = (SectionHeader *) malloc((3*sizeof(SectionHeader)));
    for (int i=0; i<section_cnt; i++){
        fread(&sections[i],sizeof(SectionHeader),1,obj_fp);
        printf("Name  %s  : Size %u  : Offset : %u\n",sections[i].section_name,sections[i].size,sections[i].offset);
    }

    for (int i=0; i< section_cnt-1; i++){
        printf("\n%s:\n",sections[i].section_name);
        print_section(sections[i].section_name,sections[i].offset,sections[i].size,obj_fp);
    }

    printf("\n");
    print_symbol_table(sections[2].size,sections[2].offset,obj_fp);
}

int main(int argc, char *argv[]){
    read_obj(argv[1]);
}