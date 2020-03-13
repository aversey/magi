#include "tools.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>


void magi_str_lowercase(char *str)
{
    for (; str && *str; ++str) {
        *str = tolower(*str);
    }
}

char *magi_str_create_copy(const char *first, int len)
{
    char *copy = magi_str_create(len);
    memcpy(copy, first, len);
    return copy;
}

char *magi_str_create(int len)
{
    char *str = malloc(len + 1);
    str[len] = 0;
    return str;
}

void magi_str_add(char **str, int *len, int *size, char c)
{
    if (!*str) {
        *len  = 0;
        *size = 2;
        *str  = malloc(2);
    } else if (*len + 1 == *size) {
        *size *= 2;
        *str   = realloc(*str, *size);
    }
    (*str)[*len]   = c;
    (*str)[++*len] = 0;
}
