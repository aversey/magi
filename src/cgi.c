#include "cgi.h"

#include "cookie.h"
#include "error.h"
#include "field.h"
#include "multipart.h"
#include "param.h"
#include "request.h"
#include "urlencoded.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char ** environ;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CGI Request Handling
 */
/* Helpers for CGI Request Handling */
static int plain_env(char ** dest, char * env_name)
{
    int          ok  = 1;
    const char * env = getenv(env_name);
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

static int lower_env(char ** dest, char * env_name)
{
    int ok = plain_env(dest, env_name);
    lowercase(*dest);
    return ok;
}

static int cgi_http_env(struct magi_request * r)
{
    int     ok     = 1;
    char ** env    = environ;
    r->http_params = 0;
    while (*env) {
        if (!strncmp(*env, "HTTP_", 5) && strncmp(*env, "HTTP_COOKIE=", 12)) {
            struct magi_param param;
            /* At least one '=' must be in *env, according to format. */
            char * name_end = strchr(*env, '=');
            param.name      = str_alloc(name_end - *env - 5);
            if (param.name) {
                memcpy(param.name, *env + 5, name_end - *env - 5);
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

static void cgi_env(struct magi_error ** e, struct magi_request * r)
{
    cgi_http_env(e, r);
    lower_env(e, &r->method, "REQUEST_METHOD");
    plain_env(e, &r->uri, "REQUEST_URI");
    plain_env(e, &r->document_root, "DOCUMENT_ROOT");
    plain_env(e, &r->document_uri, "DOCUMENT_URI");
    plain_env(e, &r->script_name, "SCRIPT_NAME");
    plain_env(e, &r->script_filename, "SCRIPT_FILENAME");
    plain_env(e, &r->remote_addr, "REMOTE_ADDR");
    plain_env(e, &r->remote_port, "REMOTE_PORT");
    plain_env(e, &r->server_addr, "SERVER_ADDR");
    lower_env(e, &r->server_name, "SERVER_NAME");
    plain_env(e, &r->server_port, "SERVER_PORT");
    lower_env(e, &r->server_protocol, "SERVER_PROTOCOL");
    plain_env(e, &r->server_software, "SERVER_SOFTWARE");
    plain_env(e, &r->path_info, "PATH_INFO");
}

static void cgi_cookies(struct magi_error **       e,
                        struct magi_cookie_list ** list)
{
    const char * env = getenv("HTTP_COOKIE");
    *list            = 0;
    if (env && *env) {
        magi_parse_cookie(e, list, env);
    } else {
        *list = 0;
    }
}

static int cgi_input_get(char ** input)
{
    int          ok        = 1;
    const char * env_input = getenv("QUERY_STRING");
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

static int cgi_input_post(char ** input, int max)
{
    int ok        = 1;
    int input_len = strtoul(getenv("CONTENT_LENGTH"), 0, 10);
    if (input_len && (input_len < max || !max)) {
        *input = str_alloc(input_len);
        if (*input) {
            if (fread(*input, 1, input_len, stdin) != input_len) {
                ok = 0;
                magi_error_set("[request:cgi] Content-length is not correct.");
            }
        } else {
            ok = 0;
        }
    }
    return ok;
}

static char * bound(const char * type)
{
    char * res = 0;
    type       = strchr(type, '=');
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

static int intput_getter(void * any)
{
    return getchar();
}

/* Interfacial CGI Request Handling */
int magi_cgi_request(struct magi_request * request,
                     void (*callback)(struct magi_field * field, char * buffer,
                                      int len),
                     int max_post)
{
    request->fields        = 0;
    request->error         = 0;
    struct magi_error ** e = &request->error;
    cgi_env(e, request);
    cgi_cookies(e, &request->cookies);
    if (request->method) {
        if (!strcmp(request->method, "post")) {
            const char * t = getenv("CONTENT_TYPE");
            if (t) {
                if (!strncmp(t, "multipart/form-data", 19)) {
                    char * boundary = bound(t);
                    if (boundary && *boundary) {
                        magi_parse_multipart(e, &request->fields,
                                             intput_getter, 0, boundary,
                                             callback);
                    } else {
                        magi_error_add(
                            e, "[request:cgi] Multipart bound is not set.");
                    }
                    free(boundary);
                } else if (!strcmp(t, "application/x-www-form-urlencoded")) {
                    char * in = 0;
                    cgi_input_post(e, &in, max_post);
                    magi_parse_urlencoded(e, &request->fields, in);
                    free(in);
                } else {
                    magi_error_add(e, "[request:cgi] Unknown content type.");
                }
            } else {
                magi_error_add(e, "[request:cgi] Content-type is not set.");
            }
        } else if (!strcmp(request->method, "get")) {
            char * in = 0;
            cgi_input_get(e, &in);
            magi_parse_urlencoded(e, &request->fields, in);
            free(in);
        }
    }
    return !request->error;
}
