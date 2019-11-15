#include "utils.h"

#include <ctype.h>
#include <stdlib.h>


void magi_str_lowercase(char * str)
{
    if (str) {
        while (*str) {
            *str = tolower(*str);
            ++str;
        }
    }
}

char * magi_str_create_copy(const char * begin, const char * end)
{
    char * res;
    res = malloc(end - begin + 1);
    if (res) {
        memcpy(res, begin, end - begin);
        res[end - begin] = 0;
    } else {
        magi_log("Cannot allocate string.");
    }
    return res;
}

char * magi_str_create(int len)
{
    char * str = malloc(len + 1);
    if (str) {
        str[len] = 0;
    } else {
        magi_log("Cannot allocate string.");
    }
    return str;
}

int magi_str_add(char ** dest, int * len, int * size, char c)
{
    int ok = 1;
    if (*len + 1 == *size) {
        *size *= 2;
        *dest = realloc(*dest, *size);
    }
    if (*dest == 0) {
        ok = 0;
        magi_log("Cannot allocate string.");
    } else {
        (*dest)[*len] = c;
        ++*len;
        (*dest)[*len] = 0;
    }
    return ok;
}