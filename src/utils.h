#ifndef MAGI_INCLUDED_UTILS
#define MAGI_INCLUDED_UTILS


static void lowercase(char *str)
{
    if (str) {
        while (*str) {
            *str = tolower(*str);
            ++str;
        }
    }
}

static char *create_str(const char *begin, const char *end)
{
    char *res;
    res = malloc(end - begin + 1);
    if (res) {
        memcpy(res, begin, end - begin);
        res[end - begin] = 0;
    }
    return res;
}

static char *str_alloc(int len)
{
    char *str = malloc(len + 1);
    if (str) {
        str[len] = 0;
    } else {
        magi_log("[request] Cannot allocate string.");
    }
    return str;
}


#endif
