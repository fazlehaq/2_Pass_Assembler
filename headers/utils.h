int get_size_in_bytes(char bytes);
int check_number_size(long number, int size);
int parsenum(char *num_str);
int strip_extra_chars(char *str);
int get_register_number(char *reg);
char * get_register_encoding( char *reg);
unsigned char make_mod_rm_byte(char* mod,char* reg,char* rm);
void to_little_endian(char *buffer,int value,int size);
int get_number_size(int number);
char * to_lowercase_in_place(char *str);
void to_lowercase(char *ip,char *op);
void append_file(FILE *dest,FILE *src);
void ensure_file_opened(FILE *fp,char *filename);
union type_conversion
{
    int i;
    unsigned int ui;
    char byte[4];
};