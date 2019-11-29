#include "response.h"

#include "utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int vsnprintf(char * s, size_t n, const char * format, va_list arg);


void magi_response_setup(struct magi_response * response)
{
    response->cookies      = 0;
    response->http_params  = 0;
    response->content_type = 0;
    response->content      = magi_str_create(0);
    response->len          = 0;
    response->size         = 1;
}


void magi_response_content_type(struct magi_response * response,
                                enum magi_content_type type)
{
    const char * const messages[] = {
        "Content-Type: application/xhtml+xml", /* magi_xhtml */
    };
    if (!response->content_type) {
        const char * end       = messages[type] + strlen(messages[type]);
        response->content_type = magi_str_create_copy(messages[type], end);
    }
}

void magi_response_add(struct magi_response * r, const char * addon)
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

void magi_response_add_format(struct magi_response * response,
                              const char *           addon,
                              ...)
{
    char *  act_addon;
    int     n;
    va_list args;
    va_start(args, addon);
    n = vsnprintf(0, 0, addon, args);
    va_end(args);
    if (n >= 0) {
        act_addon = malloc(n + 1);
        va_start(args, addon);
        vsnprintf(act_addon, n + 1, addon, args);
        va_end(args);
        magi_response_add(response, act_addon);
        free(act_addon);
    }
}

void magi_response_cookie(struct magi_response * response,
                          struct magi_cookie *   cookie)
{
    magi_cookie_list_add(&response->cookies, cookie);
}

void magi_response_cookie_easy(struct magi_response * response,
                               const char *           name,
                               const char *           value)
{
    struct magi_cookie cookie = { 0, 0, 0, 0, 0 };
    cookie.name = magi_str_create_copy(name, name + strlen(name));
    cookie.data = magi_str_create_copy(value, value + strlen(value));
    magi_cookie_list_add(&response->cookies, &cookie);
}

void magi_response_http(struct magi_response * response,
                        const char *           name,
                        const char *           data)
{
    struct magi_param param = { 0, 0 };
    param.name              = magi_str_create_copy(name, name + strlen(name));
    param.data              = magi_str_create_copy(data, data + strlen(data));
    magi_param_list_add(&response->http_params, &param);
}


void magi_response_destroy(struct magi_response * response)
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
