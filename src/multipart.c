/* Support for multifile controls are not provided. */
#include "multipart.h"

#include "error.h"
#include "param.h"
#include "tools.h"
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

static int is_str_token(char *str)
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
struct automata {
    struct magi_request *request;
    struct magi_file     file;
    struct magi_param    param;
    struct magi_param    subparam;
    char                *buf;
    int                  buf_size;
    int                  size;
    int                  len;
    char                *boundary;
    int                  boundary_pos;
    int                  boundary_len;
    int                  is_end_suspected;
    int                  is_CR_readed;
    int                  is_quoted;
    int                  readed;
    int                  newfile;
};
typedef void *(*state)(struct automata *a, char c);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Automata Shortcuts
 */
static char *extract_filename(char *n)
{
    n = strchr(n, '=');
    if (!n) {
        return 0;
    }
    n += strspn(n + 1, " \t") + 1;
    if (*n == '"') {
        ++n;
        return magi_str_create_copy(n, strchr(n, '"') - n);
    } else {
        return magi_str_create_copy(n, strcspn(n, " \t"));
    }
}

static int content_disposition(struct automata *a)
{
    char *n = strchr(a->subparam.data, '=');
    if (!n) {
        return 0;
    }
    n += strspn(n + 1, " \t") + 1;
    if (*n == '"') {
        ++n;
        a->param.name = magi_str_create_copy(n, strchr(n, '"') - n);
        if (!a->param.name || !*a->param.name) {
            return 0;
        }
    } else {
        a->param.name = magi_str_create_copy(n, strcspn(n, " \t"));
        if (!a->param.name || !is_str_token(a->param.name)) {
            return 0;
        }
    }
    a->file.filename = extract_filename(n);
    if (a->file.filename) {
        a->file.field  = a->param.name;
        a->file.params = 0;
        a->param.name  = 0;
    }
    free(a->subparam.name);
    free(a->subparam.data);
    a->subparam.name = 0;
    a->subparam.data = 0;
    return 1;
}

static int subparam_end(struct automata *a)
{
    a->size = 1;
    a->len  = 0;
    magi_str_lowercase(a->subparam.name);
    if (!strcmp(a->subparam.name, "content-disposition")) {
        return content_disposition(a);
    }
    magi_params_add(&a->file.params, &a->subparam);
    a->subparam.name = 0;
    a->subparam.data = 0;
    return 1;
}

static int param_end(struct automata *a)
{
    if (a->file.filename) {
        a->request->callback.act(a->request->callback.userdata,
                                 a->newfile, &a->file, a->buf, a->buf_size);
        a->request->callback.act(a->request->callback.userdata,
                                 0, &a->file, 0, 0);
        a->newfile  = 1;
        a->readed  -= a->buf_size;
        a->buf_size = 0;
        magi_files_add(&a->request->files, &a->file);
        a->file.filename = 0;
        a->file.field    = 0;
        a->file.params   = 0;
        a->size          = 1;
        a->len           = 0;
        return 1;
    }
    if (!a->param.name) {
        a->request->error = magi_error_multipart;
        return 0;
    }
    magi_params_add(&a->request->body, &a->param);
    a->param.name = 0;
    a->param.data = 0;
    a->size       = 1;
    a->len        = 0;
    return 1;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Boundary Interfaces
 */
static char sepget(const struct automata *a)
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

static char endget(const struct automata *a)
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

static int seplen(const struct automata *a)  { return a->boundary_len + 6; }
static int endlen(const struct automata *a)  { return a->boundary_len + 8; }

static int is_semiend(const struct automata *a)
{  /* Is end readed, expect last two chars, which are CR LF? */
    return a->is_end_suspected && (a->boundary_pos == endlen(a) - 2);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Data Reading
 */
static void apply_callback(struct automata *a)
{
    int full = a->buf_size == a->request->callback.addon_max;
    if (a->file.filename && full) {
        a->request->callback.act(a->request->callback.userdata,
                                 a->newfile, &a->file, a->buf, a->buf_size);
        a->newfile  = 0;
        a->readed  -= a->buf_size;
        a->buf_size = 0;
    }
}

static void *state_pname_pre(struct automata *a, char c);
static void *state_data(struct automata *a, char c);
static void *data_add_act(struct automata *a,
                          char             c,
                          char           **dest,
                          int             *l,
                          int             *s)
{
    int pos = a->boundary_pos;
    for (a->boundary_pos = 0; a->boundary_pos < pos; ++a->boundary_pos) {
        if (a->is_end_suspected) {
            magi_str_add(dest, l, s, endget(a));
        } else {
            magi_str_add(dest, l, s, sepget(a));
        }
        apply_callback(a);
    }
    a->boundary_pos     = 0;
    a->is_end_suspected = 0;

    if (sepget(a) != c) {
        magi_str_add(dest, l, s, c);
        apply_callback(a);
    } else {
        a->boundary_pos++;
        if (a->boundary_pos == seplen(a)) {
            param_end(a);
            return state_pname_pre;
        }
    }
    return state_data;
}

static void *data_add(struct automata *a, char c)
{
    if (a->file.filename) {
        int max = a->request->callback.addon_max + 1;
        return data_add_act(a, c, &a->buf, &a->buf_size, &max);
    } else {
        return data_add_act(a, c, &a->param.data, &a->len, &a->size);
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * State analysers
 */
static void *state_begin(struct automata *a, char c)
{
    if (sepget(a) != c) {     /* c is not wanted character from separator; */
        a->boundary_pos = 0;  /* so nullify progress in reading separator. */
    } else {
        a->boundary_pos++;
        if (a->boundary_pos == seplen(a)) {
            return state_pname_pre;  /* Separator is completed, so move on. */
        }
    }
    return state_begin;
}

static void *state_pname(struct automata *a, char c);
static void *state_pname_pre(struct automata *a, char c)
{
    if (a->is_CR_readed) {
        if (c != '\n') {
            return 0;
        }
        a->is_CR_readed = 0;
        a->boundary_pos = 0;
        return state_data;
    } else if (c == '\r') {
        a->is_CR_readed = 1;
        return state_pname_pre;
    } else if (is_token(c)) {
        magi_str_add(&a->subparam.name, &a->len, &a->size, c);
        return state_pname;
    }
    return 0;
}

static void *state_pdata(struct automata *a, char c);
static void *state_pname_end(struct automata *a, char c);
static void *state_pname(struct automata *a, char c)
{
    if (c == ':') {
        a->len  = 0;
        a->size = 1;
        return state_pdata;
    } else if (c == ' ' || c == '\t') {
        return state_pname_end;
    } else if (is_token(c)) {
        magi_str_add(&a->subparam.name, &a->len, &a->size, c);
        return state_pname;
    }
    return 0;
}

static void *state_pname_end(struct automata *a, char c)
{
    if (c == ':') {
        a->len  = 0;
        a->size = 1;
        return state_pdata;
    } else if (c == ' ' || c == '\t') {
        return state_pname_end;
    }
    return 0;
}

static void *state_pdata(struct automata *a, char c)
{
    if (a->is_CR_readed) {
        a->is_CR_readed = 0;
        if (c == '\n') {
            return subparam_end(a) ? state_pname_pre : 0;
        }
        magi_str_add(&a->subparam.data, &a->len, &a->size, '\r');
        magi_str_add(&a->subparam.data, &a->len, &a->size, c);
    } else if (c == '\r') {
        a->is_CR_readed = 1;
    } else {
        magi_str_add(&a->subparam.data, &a->len, &a->size, c);
    }
    return state_pdata;
}

static void *state_end(struct automata *a, char c);
static void *state_data(struct automata *a, char c)
{
    if (a->is_end_suspected) {
        if (endget(a) != c) {
            return data_add(a, c);
        } else {
            a->boundary_pos++;
            if (a->boundary_pos == endlen(a)) {
                param_end(a);
                return state_end;
            }
        }
    } else if (sepget(a) == c) {
        a->boundary_pos++;
        if (a->boundary_pos == seplen(a)) {
            param_end(a);
            return state_pname_pre;
        }
    } else if ((a->boundary_pos == seplen(a) - 2) && endget(a) == c) {
        a->is_end_suspected = 1;
        a->boundary_pos++;
    } else {
        return data_add(a, c);
    }
    return state_data;
}

static void *state_end(struct automata *a, char c)  { return state_end; }


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Automata Runner
 */
static void run_automata(struct automata *a,
                         int            (*next)(void *next_userdata),
                         void            *next_userdata)
{
    state s;
    int   c    = next(next_userdata);
    int   maxr = a->request->limits.params_body;
    for (s = state_begin;
         s && s != state_end && c != EOF && (!maxr || a->readed != maxr);
         c = next(next_userdata)) {
        s = s(a, c);
        ++a->readed;
    }
    if (maxr && a->readed == maxr) {
        a->request->error = magi_error_limit;
    } else if (s == state_data && is_semiend(a)) {
        param_end(a);
    } else if (s != state_end) {
        a->request->error = magi_error_multipart;
        free(a->subparam.name);
        free(a->subparam.data);
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Automata Interfaces
 */
void magi_parse_multipart(struct magi_request *request,
                          char                *boundary,
                          int                (*next)(void *userdata),
                          void                *next_userdata)
{
    struct automata a = {
        0, { 0, 0, 0 }, { 0, 0 }, { 0, 0 }, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 1
    };
    a.request      = request;
    a.boundary     = boundary;
    a.boundary_len = strlen(boundary);
    a.buf          = malloc(request->callback.addon_max + 1);
    run_automata(&a, next, next_userdata);
    free(a.buf);
}
