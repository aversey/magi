#include <magi.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int emptyurl(const char *url)
{
    return !url || !*url || (url[0] == '/' && !url[1]);
}

static int equrl(const char *a, const char *b)
{
    if (!a) return !b;
    if (!b) return 0;
    while (*a && *b && *a == *b) {
        ++a;
        ++b;
    }
    return emptyurl(a) && emptyurl(b);
}

static char *scopy(const char *s)
{
    int   len = s ? strlen(s) : 0;
    char *res  = malloc(len + 1);
    if (s) memcpy(res, s, len);
    res[len] = 0;
    return res;
}

static char *scat(const char *a, const char *b)
{
    int   alen = a ? strlen(a) : 0;
    int   blen = b ? strlen(b) : 0;
    char *res  = malloc(alen + blen + 1);
    memcpy(res, a, alen);
    memcpy(res + alen, b, blen);
    res[alen + blen] = 0;
    return res;
}


static char *nextstyle(const char *curstyle)
{
    DIR *d = opendir("/var/web/veresov.pro/public/styles");
    struct dirent *de;
    struct dirent *res = readdir(d);
    while (res->d_name[0] == '.') res = readdir(d);
    while ((de = readdir(d)) != 0) {
        if (de->d_name[0] == '.') continue;
        if (strcmp(de->d_name, curstyle)) {
            if (de->d_name[0] != '.') res = de;
            else break;
        }
    }
    closedir(d);
    return scopy(res->d_name);
}

static void switchstyle(const char *style, const char *script, const char *r)
{
    struct magi_cookie   stl = { 0, 0, 0, 0, 0 };
    struct magi_response res;
    magi_response_init(&res);
    stl.name = scopy("style");
    stl.data = nextstyle(style);
    stl.path = scopy(script);
    magi_response_cookie_complex(&res, &stl);
    if (r) {
        magi_response_status(&res, 302, "Moved Temporarily");
        magi_response_header(&res, "Location", r);
    }
    magi_response_send(&res);
    magi_response_free(&res);
    free(stl.name);
    free(stl.data);
    free(stl.path);
}

static void getstyle(const char *style, const char *script)
{
    char  c;
    char *p   = scat("/var/web/veresov.pro/public/styles/", style);
    FILE *f   = fopen(p, "r");
    struct magi_cookie   stl = { 0, 0, 0, 0, 0 };
    struct magi_response res;
    magi_response_init(&res);
    stl.name = scopy("style");
    stl.data = scopy(style);
    stl.path = scopy(script);
    magi_response_cookie_complex(&res, &stl);
    magi_response_content_type(&res, "text/css");
    magi_response_send(&res);
    magi_response_free(&res);
    while (fread(&c, 1, 1, f) == 1) {
        fwrite(&c, 1, 1, stdout);
    }
    fclose(f);
    free(p);
    free(stl.name);
    free(stl.data);
    free(stl.path);
}


static void response(struct magi_request *req)
{
    const char *curstyle = magi_request_cookie(req, "style");
    if (!curstyle) curstyle = "default.css";
    if (equrl(req->path, "/switch")) {
        switchstyle(curstyle, req->script, magi_request_param(req, "r"));
    } else if (equrl(req->path, "/style.css")) {
        getstyle(curstyle, req->script);
    }
}


int main()
{
    struct magi_request request;
    magi_request_init(&request);
    if (magi_parse(&request)) {
        response(&request);
    } else {
        magi_error_response(request.error);
    }
    magi_request_free(&request);
    return 0;
}
