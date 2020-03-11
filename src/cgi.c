#include "cgi.h"

#include "cookie.h"
#include "cookies.h"
#include "error.h"
#include "file.h"
#include "multipart.h"
#include "param.h"
#include "request.h"
#include "response.h"
#include "tools.h"
#include "urlencoded.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CGI Request
 */
static char *plain_env(char *env_name)
{
    const char *env = getenv(env_name);
    if (!env) {
        return 0;
    }
    return magi_str_create_copy(env, strlen(env));
}

static char *lower_env(char *env_name)
{
    return magi_str_lowercase(plain_env(env_name));
}

static void cgi_http_env(magi_request *r)
{
    char **env;
    int    len = 0;
    r->meta    = 0;
    for (env = environ; *env; ++env) {
        magi_param meta;
        char      *name_end;
        int        dlen;
        if (!strncmp(*env, "HTTP_COOKIE=", 12) ||
            !strncmp(*env, "QUERY_STRING=", 13)) {
            continue;
        }
        /* At least one '=' must be in *env, according to format. */
        name_end = strchr(*env, '=');
        dlen     = strlen(name_end + 1);
        len     += name_end - *env + dlen;
        if (len > r->limits.params_meta && r->limits.params_meta) {
            r->error = magi_error_limit;
            return;
        }
        meta.name = magi_str_create_copy(*env, name_end - *env);
        meta.data = magi_str_create_copy(name_end + 1, dlen);
        magi_params_add(&r->meta, &meta);
    }
}

static void cgi_env(magi_request *r)
{
    cgi_http_env(r);
    r->method        = plain_env("REQUEST_METHOD");
    r->document_root = plain_env("DOCUMENT_ROOT");
    r->script        = plain_env("SCRIPT_NAME");
    r->host          = lower_env("HTTP_HOST");
    if (!r->host) {
        r->host = lower_env("SERVER_NAME");
    }
    if (getenv("SERVER_PORT")) {
        r->port      = atoi(getenv("SERVER_PORT"));
        r->is_secure = r->port == 443;
    } else {
        r->port      = 0;
        r->is_secure = 0;
    }
    r->path = plain_env("PATH_INFO");
}

static void cgi_cookies(magi_request *r)
{
    const char *env = getenv("HTTP_COOKIE");
    if (!env || !*env) {
        r->cookies = 0;
        return;
    }
    if (strlen(env) > r->limits.cookies && r->limits.cookies) {
        r->error = magi_error_limit;
        return;
    }
    magi_parse_cookies(r, env);
}

static void cgi_input_get(magi_error *e, char **input, int max)
{
    const char *env_input = getenv("QUERY_STRING");
    if (env_input) {
        int len = strlen(env_input);
        if (len >= max && max) {
            *e = magi_error_limit;
            return;
        }
        *input = magi_str_create_copy(env_input, len);
    }
}

static void cgi_url(magi_request *request)
{
    char *in = 0;
    cgi_input_get(&request->error, &in, request->limits.params_head);
    if (!request->error) {
        magi_parse_urlencoded(&request->head, in);
        free(in);
    }
}

static void cgi_input_post(magi_error *e, char **input, int max)
{
    int input_len = strtoul(getenv("CONTENT_LENGTH"), 0, 10);
    if (!input_len) {
        *e = magi_error_length;
        return;
    }
    if (input_len > max && max) {
        *e = magi_error_limit;
        return;
    }
    *input = magi_str_create(input_len);
    if (fread(*input, 1, input_len, stdin) != input_len) {
        *e = magi_error_length;
        return;
    }
}

static char *bound(const char *type)
{
    type = strchr(type, '=');
    if (!type) {
        return 0;
    }
    type += strspn(type, " \t") + 1;
    if (*type == '"') {
        ++type;
        return magi_str_create_copy(type, type - strchr(type, '"'));
    }
    return magi_str_create_copy(type, strcspn(type, " \t"));
}

static int next()
{
    return getchar();
}

static void mhead(void *any, magi_param *header)
{
    fputs(header->name, stdout);
    fputs(": ", stdout);
    fputs(header->data, stdout);
    fputs("\r\n", stdout);
}

static void mstart_body(void *any)
{
    fputs("\r\n", stdout);
}

static void mbody(void *any, const char *data, int len)
{
    fwrite(data, 1, len, stdout);
}

static void mfile(void *any, FILE *file)
{
    while (!feof(file)) {
        char buf[64];
        int  len = fread(buf, 1, 64, file);
        fwrite(buf, 1, len, stdout);
    }
}

static void mclose(void *any)  {}

static void setup_response(magi_request *r)
{
    static const magi_response_methods m = {
        mhead,
        mstart_body,
        mbody,
        mfile,
        mclose
    };
    r->response                = malloc(sizeof(*r->response));
    r->response->methods       = &m;
    r->response->userdata      = 0;
    r->response->head_response = 0;
    r->response->head_general  = 0;
    r->response->head_entity   = 0;
    r->response->head_done     = 0;
    magi_response_content_type(r, "application/xhtml+xml");
    magi_response_status(r, 200, "OK");
}

/* Interfacial CGI Request Handling */
int magi_cgi_head(magi_request *request)
{
    request->cookies = 0;
    request->files   = 0;
    request->meta    = 0;
    request->head    = 0;
    request->body    = 0;
    request->error   = 0;
    cgi_env(request);
    cgi_cookies(request);
    cgi_url(request);
    setup_response(request);
    return !request->error;
}

int magi_cgi_body(magi_request *request)
{
    magi_error *e  = &request->error;
    request->error = magi_error_none;
    if (request->method && !strcmp(request->method, "POST")) {
        const char *t = getenv("CONTENT_TYPE");
        if (!t) {
            *e = magi_error_notype;
            return 0;
        }
        if (!strncmp(t, "multipart/form-data", 19)) {
            char *boundary = bound(t);
            if (boundary && *boundary) {
                magi_parse_multipart(request, boundary, next, 0);
            } else {
                *e = magi_error_nobound;
            }
            free(boundary);
        } else if (!strcmp(t, "application/x-www-form-urlencoded")) {
            char *in = 0;
            cgi_input_post(e, &in, request->limits.params_body);
            if (!*e) {
                magi_parse_urlencoded(&request->body, in);
            }
            free(in);
        } else {
            *e = magi_error_unknown;
            return 0;
        }
    }
    return !request->error;
}

int magi_cgi(magi_request *request)
{
    return magi_cgi_head(request) && magi_cgi_body(request);
}
