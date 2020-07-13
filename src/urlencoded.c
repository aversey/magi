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
struct automata {
    struct magi_params **list;
    char                *name;
    int                  nlen;
    int                  nsize;
    char                *data;
    int                  dlen;
    int                  dsize;
};
typedef void *(*state)(struct automata *a, char c);

static void *state_parse_data(struct automata *a, char c);
static void *state_parse_name(struct automata *a, char c)
{
    if (c == '&' || c == ';') {
        return 0;
    } else if (c == '=') {
        return deurl(&a->name) ? state_parse_data : 0;
    }
    magi_str_add(&a->name, &a->nlen, &a->nsize, c);
    return state_parse_name;
}

static void add_to_list(struct automata *a)
{
    struct magi_param param;
    param.name = a->name;
    param.data = a->data;
    magi_params_add(a->list, &param);
    a->name = 0;
    a->data = 0;
}

static void *state_parse_data(struct automata *a, char c)
{
    if (c == '=') {
        return 0;
    } else if (c == '&' || c == ';') {
        if (!deurl(&a->data)) {
            return 0;
        }
        add_to_list(a);
        return state_parse_name;
    }
    magi_str_add(&a->data, &a->dlen, &a->dsize, c);
    return state_parse_data;
}

enum magi_error magi_parse_urlencoded(struct magi_params **list,
                                      const char          *encoded)
{
    state s;
    struct automata a = { 0, 0, 0, 0, 0, 0, 0 };
    a.list = list;
    *list  = 0;
    if (!encoded || !*encoded) {
        return 0;
    }
    for (s = state_parse_name; s && *encoded; ++encoded) {
        s = s(&a, *encoded);
    }
    if (s == state_parse_name || !s || !deurl(&a.data)) {
        return magi_error_urlenc;
    }
    add_to_list(&a);
    return 0;
}
