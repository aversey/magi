#include "urlenc.h"

#include "utils.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Local Shortcuts
 */
/* Call only if is_hex(c). */
static int from_hex(char c)
{
    if (isdigit(c)) {
        return c - '0';
    } else {
        return toupper(c) - 'A' + 10;
    }
}

/* Call only if 0 <= num && num <= 15. */
static char to_hex(int num)
{
    static const char * const hex = "0123456789ABCDEF";
    return hex[num];
}

static int is_hex(char c)
{
    return isdigit(c) || strchr("ABCDEF", toupper(c));
}

static int is_url(char c)
{
    return isalnum(c) || strchr("$-_.+ !*'(),", c);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * URL Decoding
 */
static int deurl(char ** data)
{
    char * val = *data;
    int    ti;
    int    ci;
    if (!val) {
        *data  = malloc(1);
        **data = 0;
        return 1;
    }
    for (ti = 0, ci = 0; val[ci]; ++ti, ++ci) {
        if (val[ci] == '%') {
            if (is_hex(val[ci + 1]) && is_hex(val[ci + 2])) {
                /* Since chars can be signed, arithmetics are not safe. */
                val[ti] = from_hex(val[ci + 2]);       /* 0000xxxx */
                val[ti] |= from_hex(val[ci + 1]) << 4; /* XXXXxxxx */
                ci += 2; /* Two extra characters are readed from code. */
            } else {
                return 0;
            }
        } else if (val[ci] == '+') {
            val[ti] = ' ';
        } else {
            val[ti] = val[ci];
        }
    }
    val[ti] = 0;
    return 1;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Urlencoded Automata
 */
enum st { st_error = 0, st_name, st_data };

struct automata {
    struct magi_param_list ** list;
    struct magi_param         param;
    int                       size;
    int                       len;
};


static enum st parse_name(struct automata * a, char c)
{
    if (c == '&' || c == ';') { /* Impossible character means error. */
        return st_error;
    }
    if (c == '=') { /* Separator ends name. */
        a->size = 1;
        a->len  = 0;
        return st_data;
    }

    if (!magi_str_add(&a->param.name, &a->len, &a->size, c)) {
        return st_error;
    }
    return st_name;
}

static enum st end_data(struct automata * a)
{
    if (deurl(&a->param.name) && deurl(&a->param.data)) {
        if (magi_param_list_add(a->list, &a->param)) {
            a->size       = 1;
            a->len        = 0;
            a->param.name = 0;
            a->param.data = 0;
            return st_name;
        }
    }
    return st_error;
}

static enum st parse_data(struct automata * a, char c)
{
    if (c == '=') { /* Impossible character means error. */
        return st_error;
    }
    if (c == '&' || c == ';') { /* Separator ends data. */
        return end_data(a);
    }

    if (!magi_str_add(&a->param.data, &a->len, &a->size, c)) {
        return st_error;
    }
    return st_data;
}

void magi_urlenc(struct magi_param_list ** list,
                 struct magi_request *     request,
                 const char *              encoded)
{
    enum st         state;
    struct automata a = { 0, { 0, 0 }, 1, 0 };
    a.list            = list;
    *list             = 0;
    if (!encoded || !*encoded) {
        return;
    }
    for (state = st_name; state && *encoded; ++encoded) {
        if (state == st_name) {
            state = parse_name(&a, *encoded);
        } else {
            state = parse_data(&a, *encoded);
        }
    }
    if (state == st_name || !state || !end_data(&a)) {
        free(a.param.name);
        free(a.param.data);
        request->error = magi_error_urlenc;
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * URL Encoding
 */
int magi_urlenc_size(const char * plain)
{
    int size;
    if (!plain) {
        return 0;
    }
    for (size = 0; *plain; ++plain, ++size) {
        if (!is_url(*plain)) {
            size += 2;
        }
    }
    return size;
}

void magi_urlencode(const char * plain, char * code)
{
    if (!plain || !code) {
        return;
    }
    while (*plain) {
        if (is_url(*plain)) {
            *++code = *plain;
        } else {
            *++code = '%';
            *++code = to_hex(*plain & 0x0F);
            *++code = to_hex(*plain >> 4);
        }
        ++plain;
    }
}
