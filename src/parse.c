#include "parse.h"

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
    char *env = plain_env(env_name);
    magi_str_lowercase(env);
    return env;
}

static void cgi_env(struct magi_request *r)
{
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

static void cgi_cookies(struct magi_request *r)
{
    const char *env = getenv("HTTP_COOKIE");
    if (!env || !*env) {
        r->cookies = 0;
        return;
    }
    if ((int)strlen(env) > r->limits.cookies && r->limits.cookies) {
        r->error = magi_error_limit;
        return;
    }
    magi_parse_cookies(r, env);
}

static void cgi_input_get(enum magi_error *e, char **input, int max)
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

static void cgi_url(struct magi_request *request)
{
    char *in = 0;
    cgi_input_get(&request->error, &in, request->limits.params_head);
    if (!request->error) {
        magi_parse_urlencoded(&request->head, in);
        free(in);
    }
}

static void cgi_input_post(enum magi_error *e, char **input, int max)
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
    if ((int)fread(*input, 1, input_len, stdin) != input_len) {
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

static int next(void *userdata)
{
    int          size   = *(int *)userdata;
    static char *buffer = 0;
    static int   left   = 0;
    static int   last   = 0;
    static int   pos;
    if (!buffer) {
        if (last) {
            return EOF;
        }
        buffer = malloc(size);
    }
    if (!left) {
        if (last) {
            free(buffer);
            buffer = 0;
            return EOF;
        }
        left = fread(buffer, 1, size, stdin);
        last = left != size;
        pos  = 0;
    }
    left--;
    return buffer[pos++];
}

/* Interfacial CGI Request Handling */
int magi_parse_head(struct magi_request *request)
{
    request->cookies = 0;
    request->files   = 0;
    request->head    = 0;
    request->body    = 0;
    request->error   = 0;
    cgi_env(request);
    cgi_cookies(request);
    cgi_url(request);
    return !request->error;
}

int magi_parse_body(struct magi_request *request)
{
    enum magi_error *e  = &request->error;
    request->error      = magi_error_none;
    if (request->method && !strcmp(request->method, "POST")) {
        const char *t = getenv("CONTENT_TYPE");
        if (!t) {
            *e = magi_error_notype;
            return 0;
        }
        if (!strncmp(t, "multipart/form-data", 19)) {
            char *boundary = bound(t);
            if (boundary && *boundary) {
                magi_parse_multipart(request, boundary, next,
                                     &request->limits.read_buffer);
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

int magi_parse(struct magi_request *request)
{
    return magi_parse_head(request) && magi_parse_body(request);
}
