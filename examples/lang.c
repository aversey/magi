#include <magi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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


static const char *determ(const char *langprefs)
{
    int i;
    const char *langs[] = { "en", "ru" };
    const int langslen = 2;
    const char *min = 0;
    int minid = 0;
    for (i = 0; i != langslen; ++i) {
        const char *res = strstr(langprefs, langs[i]);
        if (min == 0 || res < min) {
            min = res;
            minid = i;
        }
    }
    return langs[minid];
}

static void response(struct magi_request *req)
{
    struct magi_response res;
    const char *lang = getenv("HTTP_ACCEPT_LANGUAGE");
    char *a = scat(req->script, req->path);
    char *b = scat(determ(lang), a);
    free(a);
    a = scat("/", b);
    free(b);
    b = scat(req->host, a);
    free(a);
    a = scat("https://", b);
    free(b);
    magi_response_init(&res);
    magi_response_status(&res, 302, "Moved Temporarily");
    magi_response_header(&res, "Location", a);
    free(a);
    magi_response_send(&res);
    magi_response_free(&res);
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
