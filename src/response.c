#include "response.h"

#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void magi_response_init(magi_response *r)
{
    r->head_response = 0;
    r->head_general  = 0;
    r->head_entity   = 0;
    magi_response_status(r, 200, "OK");
    magi_response_content_type(r, "text/html");
}

static void response_headers(magi_params *p)
{
    for (; p; p = p->next) {
        fputs(p->item.name, stdout);
        fputs(": ", stdout);
        fputs(p->item.data, stdout);
        fputs("\r\n", stdout);
    }
}

void magi_response_free(magi_response *r)
{
    response_headers(r->head_response);
    response_headers(r->head_general);
    response_headers(r->head_entity);
    fputs("\r\n", stdout);
    magi_params_free(r->head_response);
    magi_params_free(r->head_general);
    magi_params_free(r->head_entity);
    free(r->head_response);
    free(r->head_general);
    free(r->head_entity);
    r->head_response = 0;
    r->head_general  = 0;
    r->head_entity   = 0;
}


void magi_response_default()
{
    fputs("Status: 200 Ok\r\n"
          "Content-Type: text/html\r\n\r\n", stdout);
}


void magi_response_status(magi_response *r, int code, const char *description)
{
    int        dlen;
    magi_param addon;
    if (code <= 99 || 600 <= code) {
        return;
    }
    dlen          = strlen(description);
    addon.name    = magi_str_create_copy("Status", 6);
    addon.data    = malloc(5 + dlen);
    addon.data[0] = '0' + code / 100;
    addon.data[1] = '0' + code / 10 % 10;
    addon.data[2] = '0' + code % 10;
    addon.data[3] = ' ';
    memcpy(addon.data + 4, description, dlen + 1);
    magi_params_set(&r->head_response, &addon);
}

void magi_response_cookie(magi_response *r, const char *name, const char *data)
{
    magi_param addon;
    int        nlen;
    int        dlen;
    if (!name || !data) {
        return;
    }
    nlen       = strlen(name);
    dlen       = strlen(data);
    addon.name = magi_str_create_copy("Set-Cookie", 10);
    addon.data = malloc(nlen + dlen + 2);
    memcpy(addon.data, name, nlen);
    addon.data[nlen] = '=';
    memcpy(addon.data + nlen + 1, data, dlen + 1);
    magi_params_add(&r->head_general, &addon);
}

void magi_response_cookie_complex(magi_response *r, magi_cookie *c)
{
    magi_param addon;
    char      *pointer;
    int nlen, dlen, dsize, psize, msize;
    const int cdsize = 9;
    const int cpsize = 7;
    const int cmsize = 10;
    if (!c->name) {
        return;
    }
    nlen       = strlen(c->name);
    dlen       = c->data ? strlen(c->data) : 0;
    dsize      = c->domain ? strlen(c->domain) + cdsize : 0;
    psize      = c->path ? strlen(c->path) + cpsize : 0;
    msize      = c->max_age ? strlen(c->max_age) + cmsize : 0;
    addon.name = magi_str_create_copy("Set-Cookie", 10);
    addon.data = magi_str_create(nlen + dlen + dsize + psize + msize + 1);
    pointer    = addon.data;
    memcpy(pointer, c->name, nlen);
    pointer += nlen;
    *pointer = '=';
    ++pointer;
    if (dlen) {
        memcpy(pointer, c->data, dlen);
        pointer += dlen;
    }
    if (dsize) {
        memcpy(pointer, "; Domain=", cdsize);
        memcpy(pointer + cdsize, c->domain, dsize - cdsize);
        pointer += dsize;
    }
    if (psize) {
        memcpy(pointer, "; Path=", cpsize);
        memcpy(pointer + cpsize, c->path, psize - cpsize);
        pointer += psize;
    }
    if (msize) {
        memcpy(pointer, "; Max-Age=", cmsize);
        memcpy(pointer + cmsize, c->max_age, msize - cmsize);
    }
    magi_params_add(&r->head_general, &addon);
}

void magi_response_cookie_discard(magi_response *r, const char *name)
{
    magi_param addon;
    int len;
    if (!name) {
        return;
    }
    len        = strlen(name);
    addon.name = magi_str_create_copy("Set-Cookie", 10);
    addon.data = malloc(len + 13);
    memcpy(addon.data, name, len);
    memcpy(addon.data + len, "=; Max-Age=1", 13);
    magi_params_add(&r->head_general, &addon);
}

void magi_response_header(magi_response *r, const char *name, const char *data)
{
    magi_param addon;
    addon.name = magi_str_create_copy(name, strlen(name));
    addon.data = magi_str_create_copy(data, strlen(data));
    magi_params_add(&r->head_general, &addon);
}

void magi_response_content_length(magi_response *r, int length)
{
    magi_param addon;
    int len     = 1;
    addon.name  = magi_str_create_copy("Content-Length", 14);
    addon.data  = malloc(len + 1);
    *addon.data = '0' + length % 10;
    while (length /= 10) {
        addon.data      = realloc(addon.data, len + 2);
        addon.data[len] = '0' + length % 10;
        ++len;
    }
    addon.data[len] = 0;
    magi_params_set(&r->head_entity, &addon);
}

void magi_response_content_type(magi_response *r, const char *type)
{
    magi_param addon;
    addon.name = magi_str_create_copy("Content-Type", 12);
    addon.data = magi_str_create_copy(type, strlen(type));
    magi_params_set(&r->head_entity, &addon);
}
