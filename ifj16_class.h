class_memb_t print_fn;


#define STR(x) if(!(str = str_init( x ))) { fprintf(stderr, "ERR: Internal error.\n"); return 99; }

int populate_sym_table();

// embedded functions
void ifj16_print(string_t s);
int ifj16_readInt();
double ifj16_readDouble();
string_t ifj16_readString();
int ifj16_length(string_t s);
string_t ifj16_substr(string_t s, int i, int n);
int ifj16_compare(string_t s1, string_t s2);
int ifj16_search(string_t s, string_t search);
string_t ifj16_sort(string_t s);