#include "param.h"

#include <stdlib.h>
#include <string.h>


void magi_params_add(magi_params **params, magi_param *newitem)
{
    magi_params *node = malloc(sizeof(*node));
    if (node) {
        node->next = *params;
        node->item = *newitem;
        *params    = node;
    }
}

void magi_params_set(magi_params **params, magi_param *newitem)
{
    if (!*params) {
        magi_params_add(params, newitem);
    } else if (!strcmp((*params)->item.name, newitem->name)) {
        free((*params)->item.name);
        free((*params)->item.data);
        (*params)->item.name = newitem->name;
        (*params)->item.data = newitem->data;
    } else {
        magi_params_set(&(*params)->next, newitem);
    }
}

char *magi_params_get(const magi_params *params, const char *name)
{
    if (!params || !name) {
        return 0;
    } else if (!strcmp(params->item.name, name)) {
        return params->item.data;
    } else {
        return magi_params_get(params->next, name);
    }
}

void magi_params_free(magi_params *params)
{
    if (params) {
        magi_params_free(params->next);
        free(params->next);
        free(params->item.name);
        free(params->item.data);
    }
}
