#include "inner_tools.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>


void magi_str_lowercase(char *str)
{
    if (!str) {
        return;
    }
    for (; *str; ++str) {
        *str = tolower(*str);
    }
}

char *magi_str_create_copy(const char *first, int len)
{
    char *copy = magi_str_create(len);
    if (copy) {
        memcpy(copy, first, len);
    }
    return copy;
}

char *magi_str_create(int len)
{
    char *str = malloc(len + 1);
    if (str) {
        str[len] = 0;
    }
    return str;
}

int magi_str_add(char **dest, int *len, int *size, char c)
{
    if (!*dest) {
        *dest = magi_str_create(1);
    } else if (*len + 1 == *size) {
        *size *= 2;
        *dest  = realloc(*dest, *size);
    }
    if (!*dest) {
        return 0;
    }
    (*dest)[*len] = c;
    ++*len;
    (*dest)[*len] = 0;
    return 1;
}
