#include "param.h"

#include <stdlib.h>
#include <string.h>


int magi_param_list_add(struct magi_param_list ** list,
                        struct magi_param *       item)
{
    struct magi_param_list * node = malloc(sizeof(*node));
    if (node) {
        node->next = *list;
        node->item = *item;
        *list      = node;
    }
    return !!node;
}

char * magi_param_list_get(struct magi_param_list * list, const char * name)
{
    if (!list || !name) {
        return 0;
    } else if (!strcmp(list->item.name, name)) {
        return list->item.data;
    } else {
        return magi_param_list_get(list->next, name);
    }
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
