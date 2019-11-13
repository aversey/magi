#ifndef MAGI_INCLUDED_UTILS
#define MAGI_INCLUDED_UTILS


static void lowercase(char * str)
{
    if (str) {
        while (*str) {
            *str = tolower(*str);
            ++str;
        }
    }
}

static char * create_str(const char * begin, const char * end)
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

static char * str_alloc(int len)
{
    char * str = malloc(len + 1);
    if (str) {
        str[len] = 0;
    } else {
        magi_log("[request] Cannot allocate string.");
    }
    return str;
}

/* Shouldn't be called with 'c' as not hex digit. */
static char from_hex(char c)
{
    char num;
    if (isdigit(c)) {
        num = c - '0';
    } else {
        num = toupper(c) - 'A' + 10;
    }
    return num;
}

static int is_hex(char c)
{
    return isdigit(c) || strchr("ABCDEF", toupper(c));
}

static int add(char ** dest, int * len, int * size, char c)
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

static int is_token(char c)
{
    return 32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={} \t", c);
}

static int is_str_token(char * str)
{
    int is = str && *str; /* Empty string is not valid. */
    while (is && *str) {
        is = is_token(*str);
        ++str;
    }
    return is;
}


#endif
