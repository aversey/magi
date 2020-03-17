#include "cookies.h"

#include "tools.h"
#include <stdlib.h>
#include <string.h>


typedef enum dt { dt_plain = 0, dt_version, dt_path, dt_domain } dt;

typedef struct automata automata;
typedef void (*state)(automata *a, char c);
struct automata {
    state          s;
    magi_cookies **list;
    magi_cookie    cookie;
    char          *buf;
    int            buf_len;
    int            buf_size;
    int            is_first;
    int            is_advanced;
    int            is_quoted;
    dt             datatype;
};


static void nulify_cookie(automata *a)
{
    a->cookie.name    = 0;
    a->cookie.data    = 0;
    a->cookie.path    = 0;
    a->cookie.domain  = 0;
    a->cookie.max_age = 0;
}

static void buf_new(automata *a)
{
    a->buf      = 0;
    a->buf_len  = 0;
    a->buf_size = 1;
}


static dt what_is_name(const automata *a)
{
    dt datatype = dt_plain;
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


static void end_name(automata *a)
{
    a->datatype = what_is_name(a);
    if (a->datatype == dt_plain) {
        if (a->cookie.name) {
            magi_cookies_add(a->list, &a->cookie);
        }
        nulify_cookie(a);
        a->cookie.name = a->buf;
    } else {
        free(a->buf);
    }
    buf_new(a);
}

static int end_data(automata *a)
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

static void state_name(automata *a, char c);
static void state_pre_name(automata *a, char c)
{
    if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        a->s = state_name;
        magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c);
    } else if (c != ' ' && c != '\t'){
        a->s = 0;
    }
}

static void state_pre_data(automata *a, char c);
static void state_post_name(automata *a, char c);
static void state_name(automata *a, char c)
{
    if (c == '=') {
        a->s = state_pre_data;
        end_name(a);
    } else if (c == ' ' || c == '\t') {
        a->s = state_post_name;
        end_name(a);
    } else if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c);
    } else {
        a->s = 0;
    }
}

static void state_post_name(automata *a, char c)
{
    if (c == '=') {
        a->s = state_pre_data;
    } else if (c != ' ' && c != '\t') {
        a->s = 0;
    }
}

static void state_data(automata *a, char c);
static void state_data_quoted(automata *a, char c);
static void state_pre_data(automata *a, char c)
{
    if (c == '"') {
        a->s = state_data_quoted;
    } else if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        a->s = state_data;
        magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c);
    } else if (c != ' ' && c != '\t') {
        a->s = 0;
    }
}

static void state_post_data(automata *a, char c);
static void state_data(automata *a, char c)
{
    if (c == ';' || (c == ',' && a->is_first)) {
        a->is_first = 0;
        a->s        = end_data(a) ? state_pre_name : 0;
    } else if (c == ' ' || c == '\t') {
        a->s = end_data(a) ? state_post_data : 0;
    } else if (32 <= c && c <= 126 && !strchr("()<>@,;:\\\"/[]?={}", c)) {
        magi_str_add(&a->buf, &a->buf_len, &a->buf_size, c);
    } else {
        a->s = 0;
    }
}

static void state_data_quoted(automata *a, char c)
{
    if (c == '"') {
        a->s = end_data(a) ? state_post_data : 0;
    }
}

static void state_post_data(automata *a, char c)
{
    if (c == ';' || (c == ',' && a->is_first)) {
        a->is_first = 0;
        a->s        = state_pre_name;
    } else if (c != ' ' && c != '\t') {
        a->s = 0;
    }
}


static void parse_end(magi_error *e, automata *a)
{
    if (a->s == state_data_quoted) {
        *e = magi_error_cookies;
    } else if (a->s == state_data) {
        if (!a->cookie.name) {
            *e = magi_error_cookies;
        } else if (end_data(a)) {
            magi_cookies_add(a->list, &a->cookie);
            nulify_cookie(a);
        } else {
            *e = magi_error_cookies;
        }
    } else if (a->s != state_post_data) {
        *e = magi_error_cookies;
    }
}


void magi_parse_cookies(magi_request *request, const char *data)
{
    automata a       = { 0, 0, { 0, 0, 0, 0, 0 }, 0, 0, 0, 1, 0, 0, 0 };
    a.list           = &request->cookies;
    request->cookies = 0;
    for (a.s = state_pre_name; a.s && *data; ++data) {
        a.s(&a, *data);
    }
    parse_end(&request->error, &a);
    free(a.cookie.name);
    free(a.cookie.data);
    free(a.cookie.path);
    free(a.cookie.domain);
    free(a.buf);
}
