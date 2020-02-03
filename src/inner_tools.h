#ifndef MAGI_INCLUDED_INNER_TOOLS
#define MAGI_INCLUDED_INNER_TOOLS
/*     Magi Inner Tools
 * This file must not be included in header files intended for external use,
 * it contains some common utility functions for use in source files.
 */


void magi_str_lowercase(char *str);

/* Results of both create functions are malloced, so need to be freed. */
char *magi_str_create_copy(const char *first, int len);
char *magi_str_create(int len);

/* Null only in case of error; if *dest is null creates string. */
int magi_str_add(char **dest, int *len, int *size, char c);


#endif
