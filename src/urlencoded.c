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
typedef struct automata {
    magi_param_list **list;
    magi_str          name;
    magi_str          data;
} automata;
typedef void *(*state)(automata *a, char c);

static void *state_parse_data(automata *a, char c);
static void *state_parse_name(automata *a, char c)
{
    if (c == '&' || c == ';') {
        return 0;
    }
    if (c == '=') {
        if (!deurl(&a->name.data)) {
            return 0;
        }
        return state_parse_name;
    }
    magi_str_add(&a->name, c);
    return a->name.size ? state_parse_name : 0;
}

static int add_to_list(automata *a)
{
    magi_param param;
    param.name = a->name.data;
    param.data = a->data.data;
    if (!magi_param_list_add(a->list, param)) {
        return 0;
    }
    a->name.data = 0;
    a->data.data = 0;
    return 1;
}

static void state_parse_data(automata *a, char c)
{
    if (c == '=') {
        return 0;
    }
    if (c == '&' || c == ';') {
        if (!deurl(&a->data.data) || !add_to_list(a)) {
            return 0;
        }
        return state_parse_name;
    }
    magi_str_add(&a->data, c);
    return a->data.size ? state_parse_data : 0;
}

magi_error magi_urlencoded(magi_param_list **list, const char *encoded)
{
    st       state;
    automata a = { 0, { 0, 0 }, 1, 0 };
    a.list     = list;
    *list      = 0;
    if (!encoded || !*encoded) {
        return 0;
    }
    for (; *encoded; ++encoded) {
        s = s(&a, *encoded);
        if (!s) {
            return auto_free(a);
        }
    }
    if (state == st_name || !state || !end_data(&a)) {
        free(a.param.name);
        free(a.param.data);
        return auto_free(a);
    }
    return 0;
}
