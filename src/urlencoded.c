#include "urlencoded.h"

#include "tools.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Local Shortcuts
 */
 static int is_hex(char c)
{
    return isdigit(c) || strchr("abcdef", tolower(c));
}

/* Call only if is_hex(c). */
static int from_hex(char c)
{
    if (isdigit(c)) {
        return c - '0';
    } else {
        return tolower(c) - 'a' + 10;
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * URL Decoding
 */
static int deurl(char **data)
{
    char *val = *data;
    int   ti;
    int   ci;
    if (!val) {
        *data  = malloc(1);
        **data = 0;
        return 1;
    }
    for (ti = 0, ci = 0; val[ci]; ++ti, ++ci) {
        if (val[ci] == '%') {
            if (!is_hex(val[ci + 1]) || !is_hex(val[ci + 2])) {
                return 0;
            }
            /* Since chars can be signed, arithmetics are not safe. */
            val[ti] = from_hex(val[ci + 2]) |     /* 0000xxxx */
                      from_hex(val[ci + 1]) << 4; /* XXXXxxxx */
            ci += 2; /* Two extra characters are readed from code. */
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
typedef struct automata automata;
typedef void (*state)(automata *a, char c);
struct automata {
    state         s;
    magi_params **list;
    char         *name;
    int           nlen;
    int           nsize;
    char         *data;
    int           dlen;
    int           dsize;
};

static void state_parse_data(automata *a, char c);
static void state_parse_name(automata *a, char c)
{
    if (c == '&' || c == ';') {
        a->s = 0;
        return;
    }
    if (c == '=') {
        if (!deurl(&a->name)) {
            a->s = 0;
            return;
        }
        a->s = state_parse_data;
        return;
    }
    magi_str_add(&a->name, &a->nlen, &a->nsize, c);
}

static void add_to_list(automata *a)
{
    magi_param param;
    param.name = a->name;
    param.data = a->data;
    magi_params_add(a->list, &param);
    a->name = 0;
    a->data = 0;
}

static void state_parse_data(automata *a, char c)
{
    if (c == '=') {
        a->s = 0;
        return;
    }
    if (c == '&' || c == ';') {
        if (!deurl(&a->data)) {
            a->s = 0;
            return;
        }
        add_to_list(a);
        a->s = state_parse_name;
        return;
    }
    magi_str_add(&a->data, &a->dlen, &a->dsize, c);
}

magi_error magi_parse_urlencoded(magi_params **list, const char *encoded)
{
    automata a = { state_parse_name, 0, 0, 0, 0, 0, 0, 0 };
    a.list     = list;
    *list      = 0;
    if (!encoded || !*encoded) {
        return 0;
    }
    for (; *encoded && a.s; ++encoded) {
        a.s(&a, *encoded);
    }
    if (a.s == state_parse_name || !a.s || !deurl(&a.data)) {
        return magi_error_urlenc;
    }
    add_to_list(&a);
    return 0;
}
