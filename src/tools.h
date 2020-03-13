#ifndef MAGI_INCLUDED_TOOLS
#define MAGI_INCLUDED_TOOLS


void magi_str_lowercase(char *str);

/* Results of both create functions are malloced, so need to be freed. */
char *magi_str_create_copy(const char *first, int len);
char *magi_str_create(int len);

void magi_str_add(char **str, int *len, int *size, char c);


#endif
