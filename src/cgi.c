#include "cgi.h"

#include "cookie.h"
#include "error.h"
#include "file.h"
#include "inner_cookies.h"
#include "inner_tools.h"
#include "inner_multipart.h"
#include "inner_urlencoded.h"
#include "param.h"
#include "request.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char ** environ;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CGI Request
 */
static int plain_env(char ** dest, char * env_name)
{
    const char * env = getenv(env_name);
    if (env) {
        *dest = magi_str_create(strlen(env));
        if (!*dest) {
            return 0;
        }
        strcpy(*dest, env);
    } else {
        *dest = 0;
    }
    return 1;
}

static int lower_env(char ** dest, char * env_name)
{
    if (plain_env(dest, env_name)) {
        magi_str_lowercase(*dest);
        return 1;
    }
    return 0;
}

static int cgi_http_env(struct magi_request * r)
{
    char ** env    = environ;
    r->http_params = 0;
    while (*env) {
        if (!strncmp(*env, "HTTP_", 5) && strncmp(*env, "HTTP_COOKIE=", 12)) {
            struct magi_param param;
            /* At least one '=' must be in *env, according to format. */
            char * name_end = strchr(*env, '=');
            param.name      = magi_str_create(name_end - *env - 5);
            if (param.name) {
                memcpy(param.name, *env + 5, name_end - *env - 5);
                param.data = magi_str_create(strlen(name_end + 1));
                if (param.data) {
                    strcpy(param.data, name_end + 1);
                } else {
                    free(param.name);
                }
            }
            if (!param.name || !param.data ||
                !magi_param_list_add(&r->http_params, &param)) {
                r->error = magi_error_memory;
                return 0;
            }
        }
        ++env;
    }
    return 1;
}

static void cgi_env(struct magi_request * r)
{
    if (!cgi_http_env(r) || !lower_env(&r->method, "REQUEST_METHOD") ||
        !plain_env(&r->uri, "REQUEST_URI") ||
        !plain_env(&r->document_root, "DOCUMENT_ROOT") ||
        !plain_env(&r->document_uri, "DOCUMENT_URI") ||
        !plain_env(&r->script_name, "SCRIPT_NAME") ||
        !plain_env(&r->script_filename, "SCRIPT_FILENAME") ||
        !plain_env(&r->remote_addr, "REMOTE_ADDR") ||
        !plain_env(&r->remote_port, "REMOTE_PORT") ||
        !plain_env(&r->server_addr, "SERVER_ADDR") ||
        !lower_env(&r->server_name, "SERVER_NAME") ||
        !plain_env(&r->server_port, "SERVER_PORT") ||
        !lower_env(&r->server_protocol, "SERVER_PROTOCOL") ||
        !plain_env(&r->server_software, "SERVER_SOFTWARE") ||
        !plain_env(&r->path_info, "PATH_INFO")) {
        r->error = magi_error_memory;
    }
}

static int cgi_cookies(struct magi_request * r)
{
    const char * env = getenv("HTTP_COOKIE");
    if (!env || !*env) {
        r->cookies = 0;
        return 1;
    }
    if (strlen(env) < r->cookies_max && r->cookies_max) {
        magi_cookies(r, env);
    } else {
        r->error = magi_error_limit;
    }
    return r->error;
}

static void cgi_input_get(enum magi_error * e, char ** input, int max)
{
    const char * env_input = getenv("QUERY_STRING");
    if (env_input) {
        int len = strlen(env_input);
        if (len >= max && max) {
            *e = magi_error_limit;
            return;
        }
        *input = magi_str_create(len);
        if (!*input) {
            *e = magi_error_memory;
            return;
        }
        strcpy(*input, env_input);
    }
}

static void cgi_url(struct magi_request * request)
{
    char * in = 0;
    cgi_input_get(&request->error, &in, request->url_params_max);
    if (!request->error) {
        magi_urlencoded(&request->url_params, request, in);
    }
    free(in);
}

static void cgi_input_post(enum magi_error * e, char ** input, int max)
{
    if (!*e) {
        int input_len = strtoul(getenv("CONTENT_LENGTH"), 0, 10);
        if (!input_len) {
            *e = magi_error_length;
            return;
        }
        if (input_len >= max && max) {
            *e = magi_error_limit;
            return;
        }
        *input = magi_str_create(input_len);
        if (!*input) {
            *e = magi_error_memory;
            return;
        }
        if (fread(*input, 1, input_len, stdin) != input_len) {
            *e = magi_error_length;
            return;
        }
    }
}

static char * bound(const char * type)
{
    char * res = 0;
    type       = strchr(type, '=');
    if (type) {
        type += strspn(type, " \t") + 1;
        if (*type == '"') {
            ++type;
            res = magi_str_create_copy(type, type - strchr(type, '"'));
        } else {
            res = magi_str_create_copy(type, strcspn(type, " \t"));
        }
    }
    return res;
}

static int next(void * any)
{
    return getchar();
}

/* Interfacial CGI Request Handling */
int magi_request_cgi(struct magi_request * request)
{
    request->files       = 0;
    request->params      = 0;
    request->url_params  = 0;
    request->http_params = 0;
    request->error       = magi_error_none;
    cgi_env(request);
    cgi_cookies(request);
    cgi_url(request);
    return !request->error;
}

int magi_request_resume_cgi(struct magi_request * request)
{
    enum magi_error * e = &request->error;
    request->error      = magi_error_none;
    if (request->method && !strcmp(request->method, "post")) {
        const char * t = getenv("CONTENT_TYPE");
        if (!t) {
            *e = magi_error_notype;
            return 0;
        }
        if (!strncmp(t, "multipart/form-data", 19)) {
            char * boundary = bound(t);
            if (boundary && *boundary) {
                magi_multipart(request, boundary, next, 0);
            } else {
                *e = magi_error_nobound;
            }
            free(boundary);
        } else if (!strcmp(t, "application/x-www-form-urlencoded")) {
            char * in = 0;
            cgi_input_post(e, &in, request->params_max);
            if (!*e) {
                magi_urlencoded(&request->params, request, in);
            }
            free(in);
        } else {
            *e = magi_error_unknown;
            return 0;
        }
    }
    return !request->error;
}

int magi_request_full_cgi(magi_request *request)
{
    return magi_request_cgi(request) && magi_request_resume_cgi(request);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CGI Response
 */
void output_http_params(struct magi_param_list * list)
{
    while (list) {
        fputs(list->item.name, stdout);
        fputs(": ", stdout);
        fputs(list->item.data, stdout);
        fputs("\r\n", stdout);
        list = list->next;
    }
}

void output_cookies(struct magi_cookie_list * list)
{
    while (list) {
        fputs("Set-Cookie: ", stdout);
        fputs(list->item.name, stdout);
        fputs("=", stdout);
        fputs(list->item.data, stdout);
        if (list->item.path) {
            fputs("; Path=", stdout);
            fputs(list->item.path, stdout);
        }
        if (list->item.domain) {
            fputs("; Domain=", stdout);
            fputs(list->item.domain, stdout);
        }
        if (list->item.max_age) {
            fputs("; Max-Age=", stdout);
            fputs(list->item.max_age, stdout);
        }
        fputs("\r\n", stdout);
        list = list->next;
    }
}


int magi_response_cgi(struct magi_response * response)
{
    output_http_params(response->http_params);
    output_cookies(response->cookies);
    fputs(response->content_type, stdout);
    fputs("\r\n\r\n", stdout);
    fputs(response->content, stdout);
    return 1;
}

int magi_response_cgi_clear(magi_response *response)
{
    int ok = magi_response_cgi(response);
    magi_response_destroy(response);
    return ok;
}


int magi_error_cgi(enum magi_error error)
{
    struct magi_response res;
    magi_response_setup(&res);
    magi_response_http(&res, "Status", "400 Bad Request");
    magi_response_add_format(&res,
        "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' "
        "'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>"
        "<html xmlns='http://www.w3.org/1999/xhtml'>"
        "<head><title>400 Bad Request</title></head>"
        "<body>"
        "<h1>400 <i>Bad Request</i></h1>"
        "<h2>%s</h2>"
        "</body>"
        "</html>",
        magi_error_message(error));
    return magi_response_cgi_clear(&res);
}
