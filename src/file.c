#include "file.h"

#include <stdlib.h>
#include <string.h>


int magi_file_list_add(struct magi_file_list ** list, struct magi_file * item)
{
    struct magi_file_list * node = malloc(sizeof(*node));
    if (node) {
        node->next = *list;
        node->item = *item;
        *list      = node;
    }
    return node;
}

struct magi_file * magi_file_list_get(struct magi_file_list * list,
                                      const char *            name)
{
    if (!list || !name) {
        return 0;
    } else if (!strcmp(list->item.param_name, name)) {
        return &list->item;
    } else {
        return magi_file_list_get(list->next, name);
    }
}

void magi_file_list_destroy(struct magi_file_list * list)
{
    if (list) {
        magi_file_list_destroy(list->next);
        free(list->next);
        free(list->item.param_name);
        free(list->item.file_name);
        magi_param_list_destroy(list->item.params);
        free(list->item.params);
    }
}
