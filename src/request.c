#include "request.h"

#include "cookie.h"
#include "log.h"
#include "multipart.h"
#include "param.h"
#include "urlencoded.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern char **environ;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Local Shortcuts
 */
static void lowercase(char *str)
{
    if (str) {
        while (*str) {
            *str = tolower(*str);
            ++str;
        }
    }
}

static char *create_str(const char *begin, const char *end)
{
    char *res;
    res = malloc(end - begin + 1);
    if (res) {
        memcpy(res, begin, end - begin);
        res[end - begin] = 0;
    }
    return res;
}

static char *str_alloc(int len)
{
    char *str = malloc(len + 1);
    if (str) {
        str[len] = 0;
    } else {
        magi_log("[request] Cannot allocate string.");
    }
    return str;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Common Request Handling
 */
void magi_request_destroy(struct magi_request *request)
{
    if (request) {
        magi_field_list_destroy(request->fields);
        magi_cookie_list_destroy(request->cookies);
        free(request->fields);
        free(request->cookies);
        free(request->method);
        free(request->uri);
        free(request->document_root);
        free(request->document_uri);
        free(request->script_name);
        free(request->script_filename);
        free(request->remote_addr);
        free(request->remote_port);
        free(request->server_addr);
        free(request->server_name);
        free(request->server_port);
        free(request->server_protocol);
        free(request->server_software);
        magi_param_list_destroy(request->http_params);
        free(request->http_params);
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CGI Request Handling
 */
/* Helpers for CGI Request Handling */
static int plain_env(char **dest, char *env_name)
{
    int        ok   = 1;
    const char *env = getenv(env_name);
    if (env) {
        *dest = str_alloc(strlen(env));
        if (*dest) {
            strcpy(*dest, env);
        } else {
            ok = 0;
        }
    } else {
        *dest = 0;
    }
    return ok;
}

static int lower_env(char **dest, char *env_name)
{
    int ok = plain_env(dest, env_name);
    lowercase(*dest);
    return ok;
}

static int cgi_http_env(struct magi_request *r)
{
    int  ok        = 1;
    char **env     = environ;
    r->http_params = 0;
    while (*env) {
        if (!strncmp(*env, "HTTP_", 5) && strncmp(*env, "HTTP_COOKIE=", 12)) {
            struct magi_param param;
            /* At least one '=' must be in *env, according to format. */
            char *name_end = strchr(*env, '=');
            param.name     = str_alloc(name_end - *env);
            if (param.name) {
                memcpy(param.name, *env, name_end - *env);
                param.data = str_alloc(strlen(name_end + 1));
                if (param.data) {
                    strcpy(param.data, name_end + 1);
                } else {
                    free(param.name);
                }
            }
            if (param.name && param.data) {
                ok = magi_param_list_add(&r->http_params, &param);
            } else {
                ok = 0;
            }
        }
        ++env;
    }
    return ok;
}

static int cgi_env(struct magi_request *r)
{
    int ok = cgi_http_env(r);
    ok = ok && lower_env(&r->method,          "REQUEST_METHOD");
    ok = ok && plain_env(&r->uri,             "REQUEST_URI");
    ok = ok && plain_env(&r->document_root,   "DOCUMENT_ROOT");
    ok = ok && plain_env(&r->document_uri,    "DOCUMENT_URI");
    ok = ok && plain_env(&r->script_name,     "SCRIPT_NAME");
    ok = ok && plain_env(&r->script_filename, "SCRIPT_FILENAME");
    ok = ok && plain_env(&r->remote_addr,     "REMOTE_ADDR");
    ok = ok && plain_env(&r->remote_port,     "REMOTE_PORT");
    ok = ok && plain_env(&r->server_addr,     "SERVER_ADDR");
    ok = ok && lower_env(&r->server_name,     "SERVER_NAME");
    ok = ok && plain_env(&r->server_port,     "SERVER_PORT");
    ok = ok && lower_env(&r->server_protocol, "SERVER_PROTOCOL");
    ok = ok && plain_env(&r->server_software, "SERVER_SOFTWARE");
    ok = ok && plain_env(&r->path_info,       "PATH_INFO");
    return ok;
}

static int cgi_cookies(struct magi_cookie_list **list)
{
    int        ok   = 1;
    const char *env = getenv("HTTP_COOKIE");
    *list = 0;
    if (env && *env) {
        ok = magi_parse_cookie(list, env);
    } else {
        *list = 0;
    }
    return ok;
}

static int cgi_input_get(char **input)
{
    int        ok         = 1;
    const char *env_input = getenv("QUERY_STRING");
    if (env_input) {
        *input = str_alloc(strlen(env_input));
        if (*input) {
            strcpy(*input, env_input);
        } else {
            ok = 0;
        }
    }
    return ok;
}

static int cgi_input_post(char **input, int max)
{
    int ok        = 1;
    int input_len = strtoul(getenv("CONTENT_LENGTH"), 0, 10);
    if (input_len && (input_len < max || !max)) {
        *input = str_alloc(input_len);
        if (*input) {
            if (fread(*input, 1, input_len, stdin) != input_len) {
                ok = 0;
                magi_log("[request:cgi] Content-length is not correct.");
            }
        } else {
            ok = 0;
        }
    }
    return ok;
}

static char *bound(const char *type)
{
    char *res = 0;
    type = strchr(type, '=');
    if (type) {
        type += strspn(type, " \t") + 1;
        if (*type == '"') {
            ++type;
            res = create_str(type, strchr(type, '"'));
        } else {
            res = create_str(type, type + strcspn(type, " \t"));
        }
    }
    return res;
}

static int intput_getter(void *any)
{
    return getchar();
}


/* Interfacial CGI Request Handling */
int magi_request_build_cgi(
    struct magi_request *request,
    void (*callback)(struct magi_field *field, char *buffer, int len),
    int max_post
)
{
    int ok          = cgi_env(request) && cgi_cookies(&request->cookies);
    request->fields = 0;
    if (request->method) {
        if (!strcmp(request->method, "post")) {
            const char *t = getenv("CONTENT_TYPE");
            if (t) {
                if (!strncmp(t, "multipart/form-data", 19)) {
                    char *boundary = bound(t);
                    if (boundary && *boundary) {
                        ok = magi_parse_multipart(
                            &request->fields,
                            intput_getter,
                            0,
                            boundary,
                            callback
                        );
                    } else {
                        ok = 0;
                        magi_log("[request:cgi] Multipart bound is not set.");
                    }
                    free(boundary);
                } else if (!strcmp(t, "application/x-www-form-urlencoded")) {
                    char *in = 0;
                    ok = cgi_input_post(&in, max_post);
                    ok = ok && magi_parse_urlencoded(&request->fields, in);
                    free(in);
                } else {
                    ok = 0;
                    magi_log("[request:cgi] Unknown content type.");
                }
            } else {
                ok = 0;
                magi_log("[request:cgi] Content-type is not set.");
            }
        } else if (!strcmp(request->method, "get")) {
            char *in = 0;
            ok = cgi_input_get(&in);
            ok = ok && magi_parse_urlencoded(&request->fields, in);
            free(in);
        }
    }
    if (!ok) {
        magi_request_destroy(request);
    }
    return ok;
}
