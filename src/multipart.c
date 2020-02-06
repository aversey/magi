/* Support for multifile controls are not provided. */
#include "inner_multipart.h"

#include "error.h"
#include "inner_tools.h"
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
    struct magi_request * request;
    struct magi_file      file;
    struct magi_param     param;
    struct magi_param     subparam;
    char *                buf;
    int                   buf_size;
    int                   size;
    int                   len;
    char *                boundary;
    int                   boundary_pos;
    int                   boundary_len;
    int                   is_end_suspected;
    int                   is_CR_readed;
    int                   is_quoted;
    int                   readed;
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Automata Shortcuts
 */
static char * extract_filename(char * n)
{
    n = strchr(n, '=');
    if (!n) {
        return 0;
    }
    n += strspn(n, " \t") + 1;
    if (*n == '"') {
        ++n;
        return magi_str_create_copy(n, n - strchr(n, '"'));
    } else {
        return magi_str_create_copy(n, strcspn(n, " \t"));
    }
}

static int content_disposition(struct automata * a)
{
    char * n = strchr(a->subparam.data, '=');
    if (!n) {
        return 0;
    }
    n += strspn(n, " \t") + 1;
    if (*n == '"') {
        ++n;
        a->param.name = magi_str_create_copy(n, n - strchr(n, '"'));
        if (!a->param.name || !*a->param.name) {
            return 0;
        }
    } else {
        a->param.name = magi_str_create_copy(n, strcspn(n, " \t"));
        if (!a->param.name || !is_str_token(a->param.name)) {
            return 0;
        }
    }
    a->file.file_name = extract_filename(n);
    if (a->file.file_name) {
        a->file.param_name = a->param.name;
        a->file.params     = 0;
        a->param.name      = 0;
    }
    free(a->subparam.name);
    free(a->subparam.data);
    a->subparam.name = 0;
    a->subparam.data = 0;
    return 1;
}

static int subparam_end(struct automata * a)
{
    a->size = 1;
    a->len  = 0;
    magi_str_lowercase(a->subparam.name);
    if (!strcmp(a->subparam.name, "content-disposition")) {
        return content_disposition(a);
    } else if (magi_param_list_add(&a->file.params, &a->subparam)) {
        a->subparam.name = 0;
        a->subparam.data = 0;
        return 1;
    }
    return 0;
}

static int param_end(struct automata * a)
{
    if (a->file.file_name) {
        a->request->file_callback(&a->file, a->buf, a->buf_size, 1,
                                  a->request->file_callback_userdata);
        a->readed -= a->buf_size;
        a->buf_size = 0;
        if (!magi_file_list_add(&a->request->files, &a->file)) {
            free(a->file.file_name);
            free(a->file.param_name);
            magi_param_list_destroy(a->file.params);
            free(a->file.params);
            a->request->error = magi_error_multipart;
            return 0;
        }
        a->file.file_name  = 0;
        a->file.param_name = 0;
        a->file.params     = 0;
        a->size            = 1;
        a->len             = 0;
        return 1;
    }
    if (!a->param.name) {
        a->request->error = magi_error_multipart;
        return 0;
    }
    if (!magi_param_list_add(&a->request->params, &a->param)) {
        free(a->param.name);
        free(a->param.data);
        a->request->error = magi_error_multipart;
        return 0;
    }
    a->param.name = 0;
    a->param.data = 0;
    a->size       = 1;
    a->len        = 0;
    return 1;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Boundary Interfaces
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
 * Data Reading
 */
static void apply_callback(struct automata * a)
{
    int full = a->buf_size == a->request->file_callback_addon_max;
    if (a->file.file_name && full) {
        a->request->file_callback(&a->file, a->buf, a->buf_size, 0,
                                  a->request->file_callback_userdata);
        a->readed -= a->buf_size;
        a->buf_size = 0;
    }
}

static enum st data_add_act(
    struct automata * a, char c, char ** dest, int * len, int * size)
{
    int pos = a->boundary_pos;
    for (a->boundary_pos = 0; a->boundary_pos < pos; ++a->boundary_pos) {
        if (a->is_end_suspected) {
            magi_str_add(dest, len, size, endget(a));
        } else {
            magi_str_add(dest, len, size, sepget(a));
        }
        apply_callback(a);
    }
    a->boundary_pos     = 0;
    a->is_end_suspected = 0;

    if (sepget(a) != c) {
        magi_str_add(dest, len, size, c);
        apply_callback(a);
        return st_data;
    } else {
        a->boundary_pos++;
        if (a->boundary_pos == seplen(a)) {
            param_end(a);
            return st_pname_pre;
        }
        return st_data;
    }
}

static enum st data_add(struct automata * a, char c)
{
    if (a->file.file_name) {
        int max = a->request->file_callback_addon_max + 1;
        return data_add_act(a, c, &a->buf, &a->buf_size, &max);
    } else {
        return data_add_act(a, c, &a->param.data, &a->len, &a->size);
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * State analysers
 */
static enum st parse_begin(struct automata * a, char c)
{
    if (sepget(a) != c) {    /* 'c' is not wanted character from separator; */
        a->boundary_pos = 0; /* so nullify progress in reading separator. */
    } else {
        a->boundary_pos++;
        if (a->boundary_pos == seplen(a)) {
            return st_pname_pre; /* Separator is completed, so move on. */
        }
    }
    return st_begin;
}

static enum st parse_pname_pre(struct automata * a, char c)
{
    if (a->is_CR_readed) {
        if (c != '\n') {
            return st_error;
        }
        a->is_CR_readed = 0;
        a->boundary_pos = 0;
        return st_data;
    } else if (c == '\r') {
        a->is_CR_readed = 1;
        return st_pname_pre;
    } else if (is_token(c) &&
               magi_str_add(&a->subparam.name, &a->len, &a->size, c)) {
        return st_pname;
    }
    return st_error;
}

static enum st parse_pname(struct automata * a, char c)
{
    if (c == ':') {
        a->len  = 0;
        a->size = 1;
        return st_pdata;
    } else if (c == ' ' || c == '\t') {
        return st_pname_end;
    } else if (is_token(c) &&
               magi_str_add(&a->subparam.name, &a->len, &a->size, c)) {
        return st_pname;
    }
    return st_error;
}

static enum st parse_pname_end(struct automata * a, char c)
{
    if (c == ':') {
        a->len  = 0;
        a->size = 1;
        return st_pdata;
    } else if (c == ' ' || c == '\t') {
        return st_pname_end;
    }
    return st_error;
}

static enum st parse_pdata(struct automata * a, char c)
{
    if (a->is_CR_readed) {
        a->is_CR_readed = 0;
        if (c == '\n') {
            if (subparam_end(a)) {
                return st_pname_pre;
            }
            return st_error;
        } else if (magi_str_add(&a->subparam.data, &a->len, &a->size, '\r') &&
                   magi_str_add(&a->subparam.data, &a->len, &a->size, c)) {
            return st_pdata;
        }
        return st_error;
    } else if (c == '\r') {
        a->is_CR_readed = 1;
        return st_pdata;
    } else if (magi_str_add(&a->subparam.data, &a->len, &a->size, c)) {
        return st_pdata;
    }
    return st_error;
}

static enum st parse_data(struct automata * a, char c)
{
    if (a->is_end_suspected) {
        if (endget(a) != c) {
            return data_add(a, c);
        }
        a->boundary_pos++;
        if (a->boundary_pos == endlen(a)) {
            param_end(a);
            return st_end;
        }
        return st_data;
    } else if (sepget(a) == c) {
        a->boundary_pos++;
        if (a->boundary_pos == seplen(a)) {
            param_end(a);
            return st_pname_pre;
        }
        return st_data;
    } else if ((a->boundary_pos == seplen(a) - 2) && endget(a) == c) {
        a->is_end_suspected = 1;
        a->boundary_pos++;
        return st_data;
    }
    return data_add(a, c);
}

static enum st parse_end(struct automata * a, char c)
{
    return st_end;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Automata Runner
 */
static void run_automata(struct automata * a,
                         int (*next)(void * next_userdata),
                         void * next_userdata)
{
    enum st state = st_begin;
    int     c     = next(next_userdata);
    while (state && state != st_end && c != EOF &&
           (!a->request->params_max || a->readed != a->request->params_max)) {
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
        c = next(next_userdata);
        ++a->readed;
    }
    if (a->request->params_max && a->readed == a->request->params_max) {
        a->request->error = magi_error_limit;
        return;
    }
    if (state == st_data && is_semiend(a)) {
        state = st_end;
        param_end(a);
    }
    if (state != st_end) {
        a->request->error = magi_error_multipart;
        free(a->subparam.name);
        free(a->subparam.data);
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Automata Interfaces
 */
void magi_multipart(struct magi_request * request,
                    char *                boundary,
                    int (*next)(void * userdata),
                    void * next_userdata)
{
    struct automata a = { 0, { 0, 0, 0 }, { 0, 0 }, { 0, 0 }, 0, 0, 1,
                          0, 0,           2,        0,        0, 0, 0 };
    a.request         = request;
    a.boundary        = boundary;
    a.boundary_len    = strlen(boundary);
    a.buf             = malloc(request->file_callback_addon_max + 1);
    if (a.buf) {
        run_automata(&a, next, next_userdata);
        free(a.buf);
    }
}
