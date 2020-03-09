#include "cookie.h"

#include <stdlib.h>
#include <string.h>


void magi_cookies_add(magi_cookies **cookies, magi_cookie *newitem)
{
    magi_cookies *node = malloc(sizeof(*node));
    if (node) {
        node->next = *cookies;
        node->item = *newitem;
        *cookies   = node;
    }
}

magi_cookie *magi_cookies_get(magi_cookies *cookies, const char *name)
{
    magi_cookie *res = 0;
    if (!cookies || !name) {
        return 0;
    }
    for (; cookies; cookies = cookies->next) {
        if (!strcmp(cookies->item.name, name)) {
            res = &cookies->item;
        }
    }
    return res;
}

void magi_cookies_free(magi_cookies *cookies)
{
    if (cookies) {
        magi_cookies_free(cookies->next);
        free(cookies->next);
        free(cookies->item.name);
        free(cookies->item.data);
        free(cookies->item.path);
        free(cookies->item.domain);
        free(cookies->item.max_age);
    }
}
