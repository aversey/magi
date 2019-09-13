#include "field.h"

#include "log.h"
#include "param.h"
#include <stdlib.h>
#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Form Field
 */
int magi_field_list_add(struct magi_field_list **list, struct magi_field *item)
{
    struct magi_field_list *old  = *list;
    int ok = 1;
    *list  = malloc(sizeof(**list));
    if (*list) {
        (*list)->next = old;
        (*list)->item = *item;
    } else {
        ok = 0;
        magi_log("[field:list] Cannot allocate new list node.");
        *list = old;
    }
    return ok;
}

struct magi_field *magi_field_list_get(
    struct magi_field_list *list,
    const char *name
)
{
    struct magi_field *item = 0;
    if (list && name) {
        if (!strcmp(list->item.name, name)) {
            item = &list->item;
        } else {
            item = magi_field_list_get(list->next, name);
        }
    }
    return item;
}

void magi_field_list_destroy(struct magi_field_list *list)
{
    if (list) {
        magi_field_list_destroy(list->next);
        magi_param_list_destroy(list->item.params);
        free(list->next);
        free(list->item.name);
        free(list->item.data);
        free(list->item.params);
    }
}
