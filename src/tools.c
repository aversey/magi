#include "tools.h"

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

int magi_str_add(magi_str *str, char c)
{
    if (!str->data) {
        str->len  = 0;
        str->size = 2;
        str->data = malloc(2);
    } else if (str->len + 1 == str->size) {
        str->size *= 2;
        str->data  = realloc(str->data, str->size);
    }
    if (!str->dest) {
        str->len  = 0;
        str->size = 0;
        return 0;
    }
    str->data[str->len++] = c;
    str->data[str->len]   = 0;
    return 1;
}
