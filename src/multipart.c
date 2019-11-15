/* Support for multifile controls are not provided. */
#include "multipart.h"

#include "error.h"
#include "param.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Local Shortcuts
 */
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


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Automata for multipart/form-data
 */
enum st {
    st_error = 0,
    st_begin,
    st_pname_pre,
    st_pname,
    st_pname_end,
    st_pdata,
    st_data,
    st_end
};

struct automata {
    struct magi_field_list ** list;
    struct magi_field         field;
    struct magi_param         param;
    char *                    buf;
    int                       buf_size;
    int                       size;
    int                       len;
    char *                    boundary;
    int                       boundary_pos;
    int                       boundary_len;
    int                       is_end_suspected;
    int                       is_CR_readed;
    int                       is_quoted;
    void (*callback)(struct magi_field * field, char * buffer, int size);
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Automata Shortcuts
 */
static int content_disposition(struct automata * a)
{
    int    ok   = 1;
    char * name = strchr(a->param.data, '=');
    if (name) {
        name += strspn(name, " \t") + 1;
        if (*name == '"') {
            ++name;
            a->field.name = create_str(name, strchr(name, '"'));
            if (a->field.name == 0) {
                ok = 0;
            } else if (a->field.name[0] == 0) {
                ok = 0;
                magi_error_set(
                    "[multipart] Wrong content-disposition quotation.");
            }
        } else {
            a->field.name = create_str(name, name + strcspn(name, " \t"));
            if (a->field.name == 0) {
                ok = 0;
            } else if (!is_str_token(a->field.name)) {
                ok = 0;
                magi_error_set(
                    "[multipart] Content-disposition value is not valid, "
                    "readed: %s.",
                    a->field.name);
            }
        }
        if (ok) {
            free(a->param.name);
            free(a->param.data);
            a->param.name = 0;
            a->param.data = 0;
        }
    } else {
        ok = 0;
        magi_error_set("[multipart] Content-disposition has no '=' symbol.");
    }
    return ok;
}

static int param_end(struct automata * a)
{
    int ok = 1;
    lowercase(a->param.name);
    if (!strcmp(a->param.name, "content-disposition")) {
        ok = content_disposition(a);
    } else {
        ok            = magi_param_list_add(&a->field.params, &a->param);
        a->param.name = 0;
        a->param.data = 0;
    }
    a->size = 1;
    a->len  = 0;
    return ok;
}

static int field_end(struct automata * a)
{
    int ok;
    if (a->field.name == 0) {
        ok = 0;
        magi_error_set("[multipart] Field name is empty or not specified.");
    } else {
        if (a->callback) {
            a->callback(&a->field, a->buf, a->buf_size);
            a->buf_size = 0;
        }
        a->field.len = a->len;
        ok           = magi_field_list_add(a->list, &a->field);
        if (!ok) {
            free(a->field.name);
            free(a->field.data);
            magi_param_list_destroy(a->field.params);
            free(a->field.params);
        }
        a->field.name   = 0;
        a->field.data   = 0;
        a->field.params = 0;
        a->size         = 1;
        a->len          = 0;
    }
    return ok;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Boundary interfaces
 */
static char sepget(const struct automata * a)
{
    char      c;
    const int pos_after = a->boundary_pos - 4 - a->boundary_len;
    if (a->boundary_pos == 0) {
        c = '\r';
    } else if (a->boundary_pos == 1) {
        c = '\n';
    } else if (a->boundary_pos == 2 || a->boundary_pos == 3) {
        c = '-';
    } else if (a->boundary_pos - 4 < a->boundary_len) {
        c = a->boundary[a->boundary_pos - 4];
    } else if (pos_after == 0) {
        c = '\r';
    } else if (pos_after == 1) {
        c = '\n';
    } else {
        c = 0;
    }
    return c;
}

static int seplen(const struct automata * a)
{
    return a->boundary_len + 6;
}

static char endget(const struct automata * a)
{
    char      c;
    const int pos_after = a->boundary_pos - 4 - a->boundary_len;
    if (a->boundary_pos == 0) {
        c = '\r';
    } else if (a->boundary_pos == 1) {
        c = '\n';
    } else if (a->boundary_pos == 2 || a->boundary_pos == 3) {
        c = '-';
    } else if (a->boundary_pos - 4 < a->boundary_len) {
        c = a->boundary[a->boundary_pos - 4];
    } else if (pos_after == 0 || pos_after == 1) {
        c = '-';
    } else if (pos_after == 2) {
        c = '\r';
    } else if (pos_after == 3) {
        c = '\n';
    } else {
        c = 0;
    }
    return c;
}

static int endlen(const struct automata * a)
{
    return a->boundary_len + 8;
}

static int is_semiend(const struct automata * a)
{ /* Is end readed, expect last two chars, which are CR LF? */
    return a->is_end_suspected && (a->boundary_pos == endlen(a) - 2);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * State analysers
 */
static enum st parse_begin(struct automata * a, char c)
{
    enum st state;
    if (sepget(a) == c) {
        a->boundary_pos++;
        if (a->boundary_pos == seplen(a)) {
            state = st_pname_pre;
        } else {
            state = st_begin;
        }
    } else {
        state           = st_begin;
        a->boundary_pos = 0;
    }
    return state;
}

static enum st parse_pname_pre(struct automata * a, char c)
{
    enum st state;
    if (a->is_CR_readed) {
        a->is_CR_readed = 0;
        if (c == '\n') {
            state           = st_data;
            a->boundary_pos = 0;
        } else {
            state = st_error;
            magi_error_set(
                "[multipart] Waiting for name, CR is readed alone.");
        }
    } else if (c == '\r') {
        state           = st_pname_pre;
        a->is_CR_readed = 1;
    } else if (is_token(c)) {
        if (add(&a->param.name, &a->len, &a->size, c)) {
            state = st_pname;
        } else {
            state = st_error;
        }
    } else {
        state = st_error;
        magi_error_set(
            "[multipart] Waiting for name, readed: \\%o (render: %c).", c, c);
    }
    return state;
}

static enum st parse_pname(struct automata * a, char c)
{
    enum st state;
    if (c == ':') {
        state   = st_pdata;
        a->len  = 0;
        a->size = 1;
    } else if (c == ' ' || c == '\t') {
        state = st_pname_end;
    } else if (is_token(c)) {
        if (add(&a->param.name, &a->len, &a->size, c)) {
            state = st_pname;
        } else {
            state = st_error;
        }
    } else {
        state = st_error;
        magi_error_set("[multipart] Reading name, readed: \\%o (render: %c).",
                       c, c);
    }
    return state;
}

static enum st parse_pname_end(struct automata * a, char c)
{
    enum st state;
    if (c == ':') {
        state   = st_pdata;
        a->len  = 0;
        a->size = 1;
    } else if (c == ' ' || c == '\t') {
        state = st_pname_end;
    } else {
        state = st_error;
        magi_error_set("[multipart] Waiting for name-value separator, "
                       "readed: \\%o (render: %c).",
                       c, c);
    }
    return state;
}

static enum st parse_pdata(struct automata * a, char c)
{
    enum st state;
    if (a->is_CR_readed) {
        a->is_CR_readed = 0;
        if (c == '\n') {
            if (param_end(a)) {
                state = st_pname_pre;
            } else {
                state = st_error;
            }
        } else if (add(&a->param.data, &a->len, &a->size, '\r')) {
            if (add(&a->param.data, &a->len, &a->size, c)) {
                state = st_pdata;
            } else {
                state = st_error;
            }
        } else {
            state = st_error;
        }
    } else if (c == '\r') {
        state           = st_pdata;
        a->is_CR_readed = 1;
    } else {
        if (add(&a->param.data, &a->len, &a->size, c)) {
            state = st_pdata;
        } else {
            state = st_error;
        }
    }
    return state;
}

static void apply_callback(struct automata * a)
{
    if (a->callback && a->buf_size == magi_parse_multipart_callback_size) {
        a->callback(&a->field, a->buf, a->buf_size);
        a->buf_size = 0;
    }
}

static enum st data_add(struct automata * a, char c)
{
    static int max_buf_size = magi_parse_multipart_callback_size + 1;
    enum st    state;
    char **    dest;
    int *      len;
    int *      size;
    int        pos  = a->boundary_pos;
    state           = st_data;
    a->boundary_pos = 0;
    if (a->callback) {
        dest = &a->buf;
        len  = &a->buf_size;
        size = &max_buf_size;
    } else {
        dest = &a->field.data;
        len  = &a->len;
        size = &a->size;
    }
    while (a->boundary_pos < pos) {
        if (a->is_end_suspected) {
            add(dest, len, size, endget(a));
        } else {
            add(dest, len, size, sepget(a));
        }
        apply_callback(a);
        a->boundary_pos++;
    }
    a->boundary_pos     = 0;
    a->is_end_suspected = 0;
    if (sepget(a) == c) {
        a->boundary_pos++;
        if (a->boundary_pos == seplen(a)) {
            state = st_pname_pre;
            field_end(a);
        } else {
            state = st_data;
        }
    } else {
        add(dest, len, size, c);
        apply_callback(a);
    }
    return state;
}

static enum st parse_data(struct automata * a, char c)
{
    enum st state;
    if (a->is_end_suspected) {
        if (endget(a) == c) {
            a->boundary_pos++;
            if (a->boundary_pos == endlen(a)) {
                state = st_end;
                field_end(a);
            } else {
                state = st_data;
            }
        } else {
            state = data_add(a, c);
        }
    } else if (sepget(a) == c) {
        a->boundary_pos++;
        if (a->boundary_pos == seplen(a)) {
            state = st_pname_pre;
            field_end(a);
        } else {
            state = st_data;
        }
    } else if ((a->boundary_pos == seplen(a) - 2) && endget(a) == c) {
        state               = st_data;
        a->is_end_suspected = 1;
        a->boundary_pos++;
    } else {
        state = data_add(a, c);
    }
    return state;
}

static enum st parse_end(struct automata * a, char c)
{
    return st_end;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Automata Runner
 */
static int run_automata(struct automata * a,
                        int (*next)(void * thing),
                        void * thing)
{
    int     ok    = 1;
    enum st state = st_begin;
    int     c;
    for (c = next(thing); state && c != EOF; c = next(thing)) {
        switch (state) {
        case st_begin:
            state = parse_begin(a, c);
            break;
        case st_pname_pre:
            state = parse_pname_pre(a, c);
            break;
        case st_pname:
            state = parse_pname(a, c);
            break;
        case st_pname_end:
            state = parse_pname_end(a, c);
            break;
        case st_pdata:
            state = parse_pdata(a, c);
            break;
        case st_data:
            state = parse_data(a, c);
            break;
        case st_end:
            state = parse_end(a, c);
        default:
            break;
        }
    }
    if (state == st_data && is_semiend(a)) {
        state = st_end;
        field_end(a);
    }
    if (state != st_end) {
        ok = 0;
        if (state != st_error) {
            magi_error_set("[multipart] Input ended unexpectedly.");
        }
        free(a->field.name);
        free(a->field.data);
    }
    return ok;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Automata Interfaces
 */
int magi_parse_multipart(struct magi_field_list ** list,
                         int (*get_next)(void *),
                         void * get_next_arg,
                         char * boundary,
                         void (*callback)(struct magi_field * field,
                                          char *              buffer,
                                          int                 len))
{
    struct automata a = {
        0, { 0, 0, 0 }, { 0, 0 }, 0, 0, 1, 0, 0, 2, 0, 0, 0
    };
    int ok         = 0;
    a.list         = list;
    a.boundary     = boundary;
    a.boundary_len = strlen(boundary);
    a.callback     = callback;
    a.buf          = malloc(magi_parse_multipart_callback_size + 1);
    if (a.buf) {
        ok = run_automata(&a, get_next, get_next_arg);
        free(a.buf);
    }
    return ok;
}
