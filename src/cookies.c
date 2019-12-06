/* * *   TODO   * * */
#include "cookies.h"

#include "utils.h"
#include <stdlib.h>
#include <string.h>


enum st {
    st_error = 0,
    st_pre_name,
    st_name,
    st_post_name,
    st_pre_data,
    st_data,
    st_post_data
};

enum data_type { dt_plain = 0, dt_version, dt_path, dt_domain };

struct automata {
    struct magi_cookie_list ** list;
    struct magi_cookie         cookie;
    char *                     buf;
    int                        buf_len;
    int                        buf_size;
    int                        is_first;
    int                        is_advanced;
    int                        is_quoted;
    enum data_type             data_t;
};


static void nulify_cookie(struct automata * a)
{
    a->cookie.name    = 0;
    a->cookie.data    = 0;
    a->cookie.path    = 0;
    a->cookie.domain  = 0;
    a->cookie.max_age = 0;
}

static void buf_new(struct automata * a)
{
    a->buf      = 0;
    a->buf_len  = 0;
    a->buf_size = 1;
}


static enum data_type what_is_name(const struct automata * a)
{
    enum data_type data_t = dt_plain;
    if (a->is_first && !strcmp(a->buf, "$Version")) {
        data_t = dt_version;
    } else if (a->is_advanced) {
        if (!strcmp(a->buf, "$Path")) {
            data_t = dt_path;
        } else if (!strcmp(a->buf, "$Domain")) {
            data_t = dt_domain;
        }
    }
    return data_t;
}


static int end_name(struct automata * a)
{
    int ok    = 1;
    a->data_t = what_is_name(a);
    if (a->data_t == dt_plain) {
        if (a->cookie.name) {
            ok = magi_cookie_list_add(a->list, &a->cookie);
        }
        nulify_cookie(a);
        a->cookie.name = a->buf;
    } else {
        free(a->buf);
    }
    buf_new(a);
    return ok;
}

static int end_data(struct automata * a)
{
    int ok = 1;
    switch (a->data_t) {
    case dt_plain:
        a->cookie.data = a->buf;
        break;
    case dt_path:
        a->cookie.path = a->buf;
        break;
    case dt_domain:
        a->cookie.domain = a->buf;
        break;
    case dt_version:
        if (strcmp(a->buf, "1")) {
            ok = 0;
        }
    }
    buf_new(a);
    return ok;
}


static enum st parse_pre_name(struct automata * a, char c)
{
    enum st state;
    if (c == ' ' || c == '\t') {
        state = st_name;
    } else if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        state = st_name;
        if (!magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c)) {
            state = st_error;
        }
    } else {
        state = st_error;
    }
    return state;
}

static enum st parse_name(struct automata * a, char c)
{
    enum st state;
    if (c == '=') {
        state = st_pre_data;
        if (!end_name(a)) {
            state = st_error;
        }
    } else if (c == ' ' || c == '\t') {
        state = st_post_name;
        if (!end_name(a)) {
            state = st_error;
        }
    } else if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        state = st_name;
        if (!magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c)) {
            state = st_error;
        }
    } else {
        state = st_error;
    }
    return state;
}

static enum st parse_post_name(struct automata * a, char c)
{
    enum st state;
    if (c == '=') {
        state = st_pre_data;
    } else if (c == ' ' || c == '\t') {
        state = st_post_name;
    } else {
        state = st_error;
    }
    return state;
}

static enum st parse_pre_data(struct automata * a, char c)
{
    enum st state;
    if (c == ' ' || c == '\t') {
        state = st_pre_data;
    } else if (c == '"') {
        state        = st_data;
        a->is_quoted = 1;
    } else if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        state = st_data;
        if (!magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c)) {
            state = st_error;
        }
    } else {
        state = st_error;
    }
    return state;
}

static enum st parse_not_quoted_data(struct automata * a, char c)
{
    enum st state;
    if (c == ';' || (c == ',' && a->is_first)) {
        state       = st_pre_name;
        a->is_first = 0;
        if (!end_data(a)) {
            state = st_error;
        }
    } else if (c == ' ' || c == '\t') {
        state = st_post_data;
        if (!end_data(a)) {
            state = st_error;
        }
    } else if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        state = st_data;
        if (!magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c)) {
            state = st_error;
        }
    } else {
        state = st_error;
    }
    return state;
}

static enum st parse_data(struct automata * a, char c)
{
    enum st state;
    if (a->is_quoted) {
        if (c == '"') {
            state        = st_post_data;
            a->is_quoted = 0;
            if (!end_data(a)) {
                state = st_error;
            }
        } else {
            state = st_data;
        }
    } else {
        state = parse_not_quoted_data(a, c);
    }
    return state;
}

static enum st parse_post_data(struct automata * a, char c)
{
    enum st state;
    if (c == ';' || (c == ',' && a->is_first)) {
        state = st_pre_name;
    } else if (c == ' ' || c == '\t') {
        state = st_post_data;
    } else {
        state = st_error;
    }
    return state;
}


static void parse_end(enum magi_error * e, struct automata * a, enum st s)
{
    if (s == st_data) {
        if (a->is_quoted || !a->cookie.name) {
            *e = magi_error_cookies;
            return;
        }
        if (end_data(a) && magi_cookie_list_add(a->list, &a->cookie)) {
            nulify_cookie(a);
        } else {
            *e = magi_error_cookies;
        }
    } else if (s != st_post_data) {
        *e = magi_error_cookies;
    }
}


void magi_cookies(struct magi_request * request, const char * data)
{
    enum st         state;
    struct automata a = { 0, { 0, 0, 0, 0, 0 }, 0, 0, 1, 1, 0, 0, 0 };
    a.list            = &request->cookies;
    request->cookies  = 0;
    for (state = st_pre_name; state && *data; ++data) {
        switch (state) {
        case st_pre_name:
            state = parse_pre_name(&a, *data);
            break;
        case st_name:
            state = parse_name(&a, *data);
            break;
        case st_post_name:
            state = parse_post_name(&a, *data);
            break;
        case st_pre_data:
            state = parse_pre_data(&a, *data);
            break;
        case st_data:
            state = parse_data(&a, *data);
            break;
        case st_post_data:
            state = parse_post_data(&a, *data);
        default:
            break;
        }
    }
    parse_end(&request->error, &a, state);
    free(a.cookie.name);
    free(a.cookie.data);
    free(a.cookie.path);
    free(a.cookie.domain);
    free(a.buf);
}
