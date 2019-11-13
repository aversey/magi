#include "param.h"

#include "log.h"
#include <stdlib.h>
#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Parameter
 */
int magi_param_list_add(
    struct magi_param_list ** list, struct magi_param * item)
{
    struct magi_param_list * old = *list;
    int                      ok  = 1;
    *list                        = malloc(sizeof(**list));
    if (*list) {
        (*list)->next = old;
        (*list)->item = *item;
    } else {
        ok = 0;
        magi_log("[param:list] Cannot allocate new list node.");
        *list = old;
    }
    return ok;
}

struct magi_param * magi_param_list_get(
    struct magi_param_list * list, const char * name)
{
    struct magi_param * item = 0;
    if (list && name) {
        if (!strcmp(list->item.name, name)) {
            item = &list->item;
        } else {
            item = magi_param_list_get(list->next, name);
        }
    }
    return item;
}

void magi_param_list_destroy(struct magi_param_list * list)
{
    if (list) {
        magi_param_list_destroy(list->next);
        free(list->next);
        free(list->item.name);
        free(list->item.data);
    }
}
