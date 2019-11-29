#include "cookie.h"

#include <stdlib.h>
#include <string.h>


int magi_cookie_list_add(struct magi_cookie_list ** list,
                         struct magi_cookie *       item)
{
    struct magi_cookie_list * node = malloc(sizeof(*node));
    if (node) {
        node->next = *list;
        node->item = *item;
        *list      = node;
    }
    return !!node;
}

struct magi_cookie * magi_cookie_list_get(struct magi_cookie_list * list,
                                          const char *              name)
{
    if (!list || !name) {
        return 0;
    } else if (!strcmp(list->item.name, name)) {
        return &list->item;
    } else {
        return magi_cookie_list_get(list->next, name);
    }
}

void magi_cookie_list_destroy(struct magi_cookie_list * list)
{
    if (list) {
        magi_cookie_list_destroy(list->next);
        free(list->next);
        free(list->item.name);
        free(list->item.data);
        free(list->item.path);
        free(list->item.domain);
        free(list->item.port);
    }
}
