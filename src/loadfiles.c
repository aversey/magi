#include "loadfiles.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void loadfiles_callback(void                   *userdata,
                               int                     newfile,
                               const struct magi_file *file,
                               const char             *addon,
                               int                     addon_len)
{
    struct magi_loadfiles *loadfiles = *(struct magi_loadfiles **)userdata;
    if (!file->filename || !*file->filename) {
        return;
    }
    for (; loadfiles; loadfiles = loadfiles->next) {
        if (!strcmp(loadfiles->item->name, file->field)) {
            static FILE *f = 0;
            static int   unlimited;
            static int   left;
            if (newfile) {
                const char *path = loadfiles->item->path;
                f                = fopen(path, "wb");
                left             = loadfiles->item->max;
                unlimited        = !left;
            }
            if (unlimited) {
                fwrite(addon, 1, addon_len, f);
            } else {
                int min = left < addon_len ? left : addon_len;
                fwrite(addon, 1, min, f);
                left -= min;
            }
            if (!addon) {
                fclose(f);
            }
            return;
        }
    }
}


void magi_loadfiles_init(struct magi_loadfiles **loadfiles)
{
    *loadfiles = 0;
}

void magi_loadfiles_free(struct magi_loadfiles **loadfiles)
{
    if (!loadfiles || !*loadfiles) {
        return;
    }
    free((*loadfiles)->item);
    magi_loadfiles_free(&(*loadfiles)->next);
    free(*loadfiles);
}


void magi_loadfiles_add(struct magi_loadfiles **loadfiles,
                        const char             *name,
                        const char             *path,
                        int                     max)
{
    struct magi_loadfiles *next;
    if (!loadfiles) {
        return;
    }
    next                     = *loadfiles ? (*loadfiles)->next : 0;
    *loadfiles               = malloc(sizeof(**loadfiles));
    (*loadfiles)->item       = malloc(sizeof(struct magi_loadfile));
    (*loadfiles)->item->name = name;
    (*loadfiles)->item->path = path;
    (*loadfiles)->item->max  = max;
    (*loadfiles)->next       = next;
}


void magi_loadfiles_set(struct magi_request    *request,
                        struct magi_loadfiles **loadfiles)
{
    request->callback.act      = loadfiles_callback;
    request->callback.userdata = loadfiles;
}
