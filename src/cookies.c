#include "cookies.h"

#include "tools.h"
#include <stdlib.h>
#include <string.h>


enum dt { dt_plain = 0, dt_version, dt_path, dt_domain };

struct automata {
    struct magi_cookies **list;
    struct magi_cookie    cookie;
    char                 *buf;
    int                   buf_len;
    int                   buf_size;
    int                   is_first;
    int                   is_advanced;
    int                   is_quoted;
    enum dt               datatype;
};
typedef void *(*state)(struct automata *a, char c);


static void nullify_cookie(struct automata *a)
{
    a->cookie.name    = 0;
    a->cookie.data    = 0;
    a->cookie.path    = 0;
    a->cookie.domain  = 0;
    a->cookie.max_age = 0;
}

static void buf_new(struct automata *a)
{
    a->buf      = 0;
    a->buf_len  = 0;
    a->buf_size = 1;
}


static enum dt what_is_name(const struct automata *a)
{
    enum dt datatype = dt_plain;
    if (a->is_first && !strcmp(a->buf, "$Version")) {
        datatype = dt_version;
    } else if (a->is_advanced) {
        if (!strcmp(a->buf, "$Path")) {
            datatype = dt_path;
        } else if (!strcmp(a->buf, "$Domain")) {
            datatype = dt_domain;
        }
    }
    return datatype;
}


static void end_name(struct automata *a)
{
    a->datatype = what_is_name(a);
    if (a->datatype == dt_plain) {
        if (a->cookie.name) {
            magi_cookies_add(a->list, &a->cookie);
            a->list = &(*a->list)->next;
        }
        nullify_cookie(a);
        a->cookie.name = a->buf;
    } else {
        free(a->buf);
    }
    buf_new(a);
}

static int end_data(struct automata *a)
{
    switch (a->datatype) {
    case dt_plain:   a->cookie.data   = a->buf; break;
    case dt_path:    a->cookie.path   = a->buf; break;
    case dt_domain:  a->cookie.domain = a->buf; break;
    case dt_version: if (strcmp(a->buf, "1")) { return 0; }
    }
    buf_new(a);
    return 1;
}

static void *state_name(struct automata *a, char c);
static void *state_pre_name(struct automata *a, char c)
{
    if (c == ' ' || c == '\t') {
        return state_pre_name;
    } else if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c);
        return state_name;
    }
    return 0;
}

static void *state_pre_data(struct automata *a, char c);
static void *state_post_name(struct automata *a, char c);
static void *state_name(struct automata *a, char c)
{
    if (c == '=') {
        end_name(a);
        return state_pre_data;
    } else if (c == ' ' || c == '\t') {
        end_name(a);
        return state_post_name;
    } else if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c);
        return state_name;
    }
    return 0;
}

static void *state_post_name(struct automata *a, char c)
{
    if (c == '=') {
        return state_pre_data;
    } else if (c == ' ' || c == '\t') {
        return state_post_name;
    }
    return 0;
}

static void *state_data(struct automata *a, char c);
static void *state_data_quoted(struct automata *a, char c);
static void *state_pre_data(struct automata *a, char c)
{
    if (c == '"') {
        return state_data_quoted;
    } else if (c == ' ' || c == '\t') {
        return state_pre_data;
    } else if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c);
        return state_data;
    }
    return 0;
}

static void *state_post_data(struct automata *a, char c);
static void *state_data(struct automata *a, char c)
{
    if (c == ';' || (c == ',' && a->is_first)) {
        a->is_first = 0;
        return end_data(a) ? state_pre_name : 0;
    } else if (c == ' ' || c == '\t') {
        return end_data(a) ? state_post_data : 0;
    } else if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c);
        return state_data;
    }
    return 0;
}

static void *state_data_quoted(struct automata *a, char c)
{
    if (c == '"') {
        return end_data(a) ? state_post_data : 0;
    }
    magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c);
    return state_data_quoted;
}

static void *state_post_data(struct automata *a, char c)
{
    if (c == ';' || (c == ',' && a->is_first)) {
        a->is_first = 0;
        return state_pre_name;
    } else if (c == ' ' || c == '\t') {
        return state_post_data;
    }
    return 0;
}


static void parse_end(enum magi_error *e, struct automata *a, state s)
{
    if (s == state_data_quoted) {
        *e = magi_error_cookies;
    } else if (s == state_data) {
        if (!a->cookie.name) {
            *e = magi_error_cookies;
        } else if (end_data(a)) {
            magi_cookies_add(a->list, &a->cookie);
            nullify_cookie(a);
        } else {
            *e = magi_error_cookies;
        }
    } else if (s == state_post_data) {
        magi_cookies_add(a->list, &a->cookie);
        nullify_cookie(a);
    } else {
        *e = magi_error_cookies;
    }
}


void magi_parse_cookies(struct magi_request *request, const char *data)
{
    state           s;
    struct automata a = { 0, { 0, 0, 0, 0, 0 }, 0, 0, 0, 1, 0, 0, 0 };
    a.list            = &request->cookies;
    request->cookies  = 0;
    for (s = state_pre_name; s && *data; ++data) {
        s = s(&a, *data);
    }
    parse_end(&request->error, &a, s);
    free(a.cookie.name);
    free(a.cookie.data);
    free(a.cookie.path);
    free(a.cookie.domain);
    free(a.buf);
}
