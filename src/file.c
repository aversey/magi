#include "file.h"

#include <stdlib.h>
#include <string.h>


void magi_files_add(struct magi_files     **files,
                    const struct magi_file *newitem)
{
    struct magi_files *node = malloc(sizeof(*node));
    if (node) {
        node->next = *files;
        node->item = *newitem;
        *files     = node;
    }
}

const struct magi_file *magi_files_get(const struct magi_files *files,
                                       const char              *name)
{
    if (!files || !name) {
        return 0;
    } else if (!strcmp(files->item.field, name)) {
        return &files->item;
    } else {
        return magi_files_get(files->next, name);
    }
}

void magi_files_free(struct magi_files *files)
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


char *magi_file_param(struct magi_file *file, const char *name)
{
    return file ? magi_params_get(file->params, name) : 0;
}
