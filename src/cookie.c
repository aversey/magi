#include "cookie.h"

#include "log.h"
#include <stdlib.h>
#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Cookie Parse
 */
enum st {
    st_error = 0,
    st_pre_name,
    st_name,
    st_post_name,
    st_pre_data,
    st_data,
    st_post_data
};

enum data_type { dt_plain = 0, dt_version, dt_path, dt_domain, dt_port };

struct automata {
    struct magi_cookie_list ** list;
    struct magi_cookie         cookie;
    char *                     buf;
    int                        buf_len;
    int                        buf_size;
    int                        is_first;
    int                        is_cookie2;
    int                        is_quoted;
    enum data_type             data_t;
};

static void nulify_cookie(struct automata * a)
{
    a->cookie.name   = 0;
    a->cookie.data   = 0;
    a->cookie.path   = 0;
    a->cookie.domain = 0;
    a->cookie.port   = 0;
}

static void buf_new(struct automata * a)
{
    a->buf      = 0;
    a->buf_len  = 0;
    a->buf_size = 1;
}

static int buf_add(struct automata * a, char c)
{
    int ok = 1;
    if (a->buf_len == a->buf_size - 1) {
        a->buf_size *= 2;
        a->buf = realloc(a->buf, a->buf_size);
    }
    if (a->buf) {
        a->buf_len++;
        a->buf[a->buf_len - 1] = c;
        a->buf[a->buf_len]     = 0;
    } else {
        ok = 0;
        magi_log("[cookie] Cannot allocate automata buffer.");
    }
    return ok;
}

static enum data_type what_is_name(const struct automata * a)
{
    enum data_type data_t = dt_plain;
    if (a->is_first && !strcmp(a->buf, "$Version")) {
        data_t = dt_version;
    } else if (a->is_cookie2) {
        if (!strcmp(a->buf, "$Path")) {
            data_t = dt_path;
        } else if (!strcmp(a->buf, "$Domain")) {
            data_t = dt_domain;
        } else if (!strcmp(a->buf, "$Port")) {
            data_t = dt_port;
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
    case dt_port:
        a->cookie.port = a->buf;
        break;
    case dt_version:
        if (strcmp(a->buf, "1")) {
            ok = 0;
            magi_log("[cookie] Version must be '1', readed: %s.", a->buf);
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
        if (!buf_add(a, c)) {
            state = st_error;
        }
    } else {
        state = st_error;
        magi_log("[cookie] Pre-name, readed: \\%o (render: %c).", c, c);
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
        if (!buf_add(a, c)) {
            state = st_error;
        }
    } else {
        state = st_error;
        magi_log("[cookie] Reading name, readed: \\%o (render: %c).", c, c);
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
        magi_log("[cookie] Waiting for name-value separator, "
                 "readed: \\%o (render: %c).",
            c, c);
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
        if (!buf_add(a, c)) {
            state = st_error;
        }
    } else {
        state = st_error;
        magi_log("[cookie] Pre-value, readed: \\%o (render: %c).", c, c);
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
        if (!buf_add(a, c)) {
            state = st_error;
        }
    } else {
        state = st_error;
        magi_log("[cookie] Reading not-quoted value, "
                 "readed: \\%o (render: %c).",
            c, c);
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
        magi_log("[cookie] Waiting for separator between name-value pairs, "
                 "readed: \\%o (render: %c).",
            c, c);
    }
    return state;
}

static int parse_end(struct automata * a, enum st s)
{
    int ok = 0;
    if (s == st_post_data) {
        ok = 1;
    } else if (s == st_data) {
        if (!a->is_quoted) {
            if (a->cookie.name) {
                if (end_data(a) && magi_cookie_list_add(a->list, &a->cookie)) {
                    ok = 1;
                    nulify_cookie(a);
                    buf_new(a);
                }
            } else {
                magi_log("[cookie] No cookies set.");
            }
        } else {
            magi_log("[cookie] In quotation when reached input end.");
        }
    } else if (s != st_error) {
        magi_log("[cookie] Input ended in not correct state.");
    }
    free(a->cookie.name);
    free(a->cookie.data);
    free(a->cookie.path);
    free(a->cookie.domain);
    free(a->cookie.port);
    free(a->buf);
    return ok;
}

int magi_parse_cookie(struct magi_cookie_list ** list, const char * input)
{
    struct automata a     = { 0, { 0, 0, 0, 0, 0 }, 0, 0, 1, 1, 0, 0, 0 };
    enum st         state = st_pre_name;
    a.list                = list;
    while (*input && state) {
        char c = *input++;
        switch (state) {
        case st_pre_name:
            state = parse_pre_name(&a, c);
            break;
        case st_name:
            state = parse_name(&a, c);
            break;
        case st_post_name:
            state = parse_post_name(&a, c);
            break;
        case st_pre_data:
            state = parse_pre_data(&a, c);
            break;
        case st_data:
            state = parse_data(&a, c);
            break;
        case st_post_data:
            state = parse_post_data(&a, c);
        default:
            break;
        }
    }
    return parse_end(&a, state);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Cookie List
 */
int magi_cookie_list_add(
    struct magi_cookie_list ** list, struct magi_cookie * item)
{
    struct magi_cookie_list * old = *list;
    int                       ok  = 1;
    *list                         = malloc(sizeof(**list));
    if (*list) {
        (*list)->next = old;
        (*list)->item = *item;
    } else {
        ok = 0;
        magi_log("[cookie:list] Cannot allocate new list node.");
        *list = old;
    }
    return ok;
}

char * magi_cookie_list_get(struct magi_cookie_list * list, const char * name)
{
    char * data = 0;
    if (list && name) {
        if (!strcmp(list->item.name, name)) {
            data = list->item.data;
        } else {
            data = magi_cookie_list_get(list->next, name);
        }
    }
    return data;
}

void magi_cookie_list_destroy(struct magi_cookie_list * list)
{
    if (list) {
        magi_cookie_list_destroy(list->next);
        free(list->next);
        free(list->item.name);
        free(list->item.data);
        free(list->item.domain);
        free(list->item.path);
        free(list->item.port);
    }
}
