#ifndef MAGI_INCLUDED_TOOLS
#define MAGI_INCLUDED_TOOLS


char *magi_str_lowercase(char *str);

/* Results of both create functions are malloced, so need to be freed. */
char *magi_str_create_copy(const char *first, int len);
char *magi_str_create(int len);


typedef struct magi_str {
    char *data;
    int   size;
    int   len;
} magi_str;

int magi_str_add(magi_str *str, char c);


#endif
