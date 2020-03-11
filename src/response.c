#include "response.h"

#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void magi_response_status(magi_request *r, int code, const char *description)
{
    int        dlen;
    magi_param addon;
    if (r->response->head_done || code <= 99 || 600 <= code) {
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
    magi_params_set(&r->response->head_response, &addon);
}

void magi_response_cookie(magi_request *r, const char *name, const char *data)
{
    magi_param addon;
    if (r->response->head_done) {
        return;
    }
    addon.name = magi_str_create_copy("Set-Cookie", 10);
    /* TODO */
    magi_params_add(&r->response->head_general, &addon);
}

void magi_response_cookie_complex(magi_request *r, magi_cookie *c)
{
    magi_param addon;
    if (r->response->head_done) {
        return;
    }
    addon.name = magi_str_create_copy("Set-Cookie", 10);
    /* TODO */
    magi_params_add(&r->response->head_general, &addon);
}

void magi_response_cookie_discard(magi_request *r, const char *name)
{
    magi_param addon;
    if (r->response->head_done) {
        return;
    }
    addon.name = magi_str_create_copy("Set-Cookie", 10);
    /* TODO */
    magi_params_add(&r->response->head_general, &addon);
}

void magi_response_header(magi_request *r, const char *name, const char *data)
{
    magi_param addon;
    if (r->response->head_done) {
        return;
    }
    addon.name = magi_str_create_copy(name, strlen(name));
    addon.data = magi_str_create_copy(data, strlen(data));
    magi_params_add(&r->response->head_general, &addon);
}

void magi_response_content_length(magi_request *r, int length)
{
    magi_param addon;
    int        len;
    if (r->response->head_done) {
        return;
    }
    addon.name  = magi_str_create_copy("Content-Length", 14);
    len         = 1;
    addon.data  = malloc(len + 1);
    *addon.data = '0' + length % 10;
    while (length /= 10) {
        addon.data      = realloc(addon.data, len + 2);
        addon.data[len] = '0' + length % 10;
        ++len;
    }
    addon.data[len] = 0;
    magi_params_set(&r->response->head_entity, &addon);
}

void magi_response_content_type(magi_request *r, const char *type)
{
    magi_param addon;
    if (r->response->head_done || !type) {
        return;
    }
    addon.name = magi_str_create_copy("Content-Type", 12);
    addon.data = magi_str_create_copy(type, strlen(type));
    magi_params_set(&r->response->head_entity, &addon);
}

static void response_headers(magi_response_implementation *r, magi_params *p)
{
    for (; p; p = p->next) {
        r->methods->head(r->userdata, &p->item);
    }
}

void magi_response_head(magi_request *r)
{
    if (r->response->head_done) {
        return;
    }
    response_headers(r->response, r->response->head_response);
    response_headers(r->response, r->response->head_general);
    response_headers(r->response, r->response->head_entity);
    r->response->methods->start_body(r->response->userdata);
    r->response->head_done = 1;
}

void magi_response(magi_request *r, const char *addon)
{
    magi_response_head(r);
    if (!addon) {
        return;
    }
    r->response->methods->body(r->response->userdata, addon, strlen(addon));
}

void magi_response_format(magi_request *r, const char *format, ...)
{
    magi_response_head(r);
    /* TODO */
}

void magi_response_file(magi_request *r, FILE *file)
{
    magi_response_head(r);
    r->response->methods->file(r->response->userdata, file);
}


void magi_response_error(magi_request *r)
{
    /* TODO */
    magi_response_status(r, 400, "Bad Request");
    magi_response(r,
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
        "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><title>400 Bad Request</title></head>"
        "<body>"
        "<h1>400 <i>Bad Request</i></h1>"
        "<h2>");
    magi_response(r, magi_error_message(r->error));
    magi_response(r, "</h2></body></html>");
}
