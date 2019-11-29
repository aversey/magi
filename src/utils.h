#ifndef MAGI_INCLUDED_UTILS
#define MAGI_INCLUDED_UTILS


void magi_str_lowercase(char * str);

/* Results of both create functions are malloced, so need to be freed. */
char * magi_str_create_copy(const char * begin, const char * end);
char * magi_str_create(int len);

/* Null only in case of error. */
int magi_str_add(char ** dest, int * len, int * size, char c);


#endif
