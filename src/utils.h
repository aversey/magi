#ifndef MAGI_INCLUDED_UTILS
#define MAGI_INCLUDED_UTILS


void   magi_str_lowercase(char * str);
char * magi_str_create_copy(const char * begin, const char * end);
char * magi_str_create(int len);
char * magi_str_add(char * dest, int * len, int * size, char c);


#endif
