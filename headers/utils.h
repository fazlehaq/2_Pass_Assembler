int get_size_in_bytes(char bytes);
int check_number_size(long number, int size);
int parsenum(char *num_str);
int strip_extra_chars(char *str);
int get_register_number(char *reg);
char * get_register_encoding( char *reg);
unsigned char make_mod_rm_byte(char* mod,char* reg,char* rm);