#include "cookie.h"

#include <stdlib.h>
#include <string.h>


void magi_cookies_free(struct magi_cookies *cookies)
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

void magi_cookies_add(struct magi_cookies     **cookies,
                      const struct magi_cookie *newitem)
{
    struct magi_cookies *node = malloc(sizeof(*node));
    if (node) {
        node->next = *cookies;
        node->item = *newitem;
        *cookies   = node;
    }
}

const struct magi_cookie *magi_cookies_get(const struct magi_cookies *cookies,
                                           const char                *name)
{
    if (!name) {
        return 0;
    }
    for (; cookies; cookies = cookies->next) {
        if (!strcmp(cookies->item.name, name)) {
            return &cookies->item;
        }
    }
    return 0;
}
