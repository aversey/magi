#include "urlencoded.h"

#include "error.h"
#include "field.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Local Shortcuts
 */
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


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * URL Decoding
 */
static int deurl(char ** data)
{
    int ok = 1;
    if (*data) {
        char * val = *data;
        int    ti  = 0;
        int    ci;
        for (ci = 0; ok && val[ci]; ++ti, ++ci) {
            if (val[ci] == '%') {
                if (is_hex(val[ci + 1]) && is_hex(val[ci + 2])) {
                    /* Since chars can be signed, arithmetics are not safe. */
                    val[ti] = from_hex(val[ci + 2]);       /* 00xx */
                    val[ti] |= from_hex(val[ci + 1]) << 4; /* XXxx */
                    ci += 2;
                } else {
                    ok = 0;
                    magi_error_set(
                        "[urlencoded] Waiting for two hex digits after '%%', "
                        "readed: \\%o\\%o (render: %c%c)",
                        val[ci + 1], val[ci + 2], val[ci + 1], val[ci + 2]);
                }
            } else if (val[ci] == '+') {
                val[ti] = ' ';
            } else {
                val[ti] = val[ci];
            }
        }
        val[ti++] = 0;
    } else {
        *data  = malloc(1);
        **data = 0;
    }
    return ok;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Urlencoded Automata
 */
enum st { st_error = 0, st_name, st_data };

struct automata {
    struct magi_field_list ** list;
    struct magi_field         field;
    int                       size;
    int                       len;
};


static enum st parse_name(struct automata * a, char c)
{
    enum st state;
    if (c != '&' && c != ';') {
        if (c == '=') {
            state   = st_data;
            a->size = 1;
            a->len  = 0;
        } else {
            if (a->len == a->size - 1) {
                a->size *= 2;
                a->field.name = realloc(a->field.name, a->size);
            }
            if (!a->field.name) {
                state = st_error;
                magi_error_set("[urlencoded] Cannot allocate field name.");
            } else {
                state = st_name;
                a->len++;
                a->field.name[a->len - 1] = c;
                a->field.name[a->len]     = 0;
            }
        }
    } else {
        state = st_error;
        magi_error_set("[urlencoded] Reading name, readed: \\%o (render: %c).",
                       c, c);
    }
    return state;
}

static enum st end_data(struct automata * a)
{
    enum st state = st_error;
    if (deurl(&a->field.name) && deurl(&a->field.data)) {
        a->field.len = strlen(a->field.data);
        if (magi_field_list_add(a->list, &a->field)) {
            state         = st_name;
            a->size       = 1;
            a->len        = 0;
            a->field.name = 0;
            a->field.data = 0;
        }
    }
    return state;
}

static enum st parse_data(struct automata * a, char c)
{
    enum st state;
    if (c != '=') {
        if (c == '&' || c == ';') {
            state = end_data(a);
        } else {
            if (a->len == a->size - 1) {
                a->size *= 2;
                a->field.data = realloc(a->field.data, a->size);
            }
            if (!a->field.data) {
                state = st_error;
                magi_error_set("[urlencoded] Cannot allocate field data.");
            } else {
                state = st_data;
                a->len++;
                a->field.data[a->len - 1] = c;
                a->field.data[a->len]     = 0;
            }
        }
    } else {
        state = st_error;
        magi_error_set("[urlencoded] Reading data, readed: \\%o (render: %c).",
                       c, c);
    }
    return state;
}

int magi_parse_urlencoded(struct magi_field_list ** list, const char * input)
{
    enum st         state = st_name;
    struct automata a     = { 0, { 0, 0, 0 }, 1, 0 };
    if (input && *input) {
        a.list = list;
        while (state && *input) {
            switch (state) {
            case st_name:
                state = parse_name(&a, *input);
                break;
            case st_data:
                state = parse_data(&a, *input);
            default:
                break;
            }
            ++input;
        }
        if (state == st_data) {
            state = end_data(&a);
        } else if (state == st_name) {
            state = st_error;
            magi_error_set("[urlencoded] Input ended while reading name.");
        }
        free(a.field.name);
        free(a.field.data);
    }
    return state != st_error;
}
