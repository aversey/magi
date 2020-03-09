#include "file.h"

#include <stdlib.h>
#include <string.h>


void magi_files_add(magi_files **files, magi_file *newitem)
{
    magi_files *node = malloc(sizeof(*node));
    if (node) {
        node->next = *files;
        node->item = *newitem;
        *files     = node;
    }
}

magi_file *magi_files_get(magi_files *files, const char *name)
{
    if (!files || !name) {
        return 0;
    } else if (!strcmp(files->item.field, name)) {
        return &files->item;
    } else {
        return magi_files_get(files->next, name);
    }
}

void magi_files_free(magi_files *files)
{
    if (files) {
        magi_files_free(files->next);
        free(files->next);
        free(files->item.field);
        free(files->item.filename);
        magi_params_free(files->item.params);
        free(files->item.params);
    }
}
