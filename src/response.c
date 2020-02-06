#include "response.h"

#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void magi_response_setup(magi_response *response)
{
    response->cookies      = 0;
    response->http_params  = 0;
    response->content_type = 0;
    magi_response_content_type(response, "application/xhtml+xml");
    response->content      = magi_str_create(0);
    response->len          = 0;
    response->size         = 1;
}

void magi_response_content_type(magi_response *response, const char *type)
{
    static const char *const ct    = "Content-Type: ";
    static const int         ctlen = 15;
    const int                len   = strlen(type);
    free(response->content_type);
    response->content_type = malloc(ctlen + len + 1);
    memcpy(response->content_type, ct, ctlen);
    memcpy(response->content_type + ctlen, type, len + 1);
}

void magi_response_add(magi_response *r, const char *addon)
{
    int addon_len;
    if (!addon) {
        return;
    }
    addon_len = strlen(addon);
    if (r->len + addon_len + 1 >= r->size) {
        r->content = realloc(r->content, r->len + addon_len + 1);
    }
    memcpy(r->content + r->len, addon, addon_len + 1);
    r->len += addon_len;
}

void magi_response_cookie(magi_response *response, magi_cookie *cookie)
{
    magi_cookie_list_add(&response->cookies, cookie);
}

void magi_response_cookie_easy(magi_response *response,
                               const char    *name,
                               const char    *value)
{
    magi_cookie cookie = { 0, 0, 0, 0, 0 };
    cookie.name        = magi_str_create_copy(name, strlen(name));
    cookie.data        = magi_str_create_copy(value, strlen(value));
    magi_cookie_list_add(&response->cookies, &cookie);
}

void magi_response_cookie_discard(magi_response *response, const char *name)
{
    magi_cookie cookie = { 0, 0, 0, 0, 0 };
    cookie.name        = magi_str_create_copy(name, strlen(name));
    cookie.max_age     = magi_str_create(1);
    cookie.max_age[0]  = '0';
    magi_cookie_list_add(&response->cookies, &cookie);
}

void magi_response_http(magi_response *response,
                        const char    *name,
                        const char    *data)
{
    magi_param param = { 0, 0 };
    param.name       = magi_str_create_copy(name, strlen(name));
    param.data       = magi_str_create_copy(data, strlen(data));
    magi_param_list_add(&response->http_params, &param);
}


void magi_response_destroy(magi_response *response)
{
    if (!response) {
        return;
    }
    magi_cookie_list_destroy(response->cookies);
    magi_param_list_destroy(response->http_params);
    free(response->cookies);
    free(response->http_params);
    free(response->content_type);
    free(response->content);
}
