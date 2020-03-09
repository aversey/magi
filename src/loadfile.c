#include "loadfile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void magi_loadfiles_add(magi_loadfiles *table,
                        const char     *name,
                        const char     *path,
                        int             max)
{
    static const int size = sizeof(*table->files);
    if (!table) {
        return;
    }
    if (table->count) {
        table->files = realloc(table->files, size * table->count + size);
    } else {
        table->files = malloc(size);
    }
    table->files[table->count].name = name;
    table->files[table->count].path = path;
    table->files[table->count].max  = max;
    table->count++;
}

void magi_loadfiles_free(magi_loadfiles *table)
{
    if (!table) {
        return;
    }
    free(table->files);
    table->count = 0;
}

static void loadfiles(void *userdata,
                      magi_file *file,
                      char      *addon,
                      int        addon_len)
{
    magi_loadfiles *table = userdata;
    int             pos;
    if (!file->filename || !strcmp(file->filename, "")) {
        return;
    }
    for (pos = 0; pos != table->count; ++pos) {
        if (!strcmp(table->files[pos].name, file->field)) {
            static FILE *f = 0;
            static int   unlimited;
            static int   left;
            if (!f) {
                const char *path = table->files[pos].path;
                f                = fopen(path, "wb");
                left             = table->files[pos].max;
                unlimited        = !left;
            }
            if (unlimited) {
                fwrite(addon, 1, addon_len, f);
            } else {
                int min = left < addon_len ? left : addon_len;
                fwrite(addon, 1, min, f);
                left -= min;
            }
            if (!addon_len) {
                fclose(f);
                f = 0;
            }
            return;
        }
    }
}

void magi_loadfiles_set(magi_request *request, magi_loadfiles *table)
{
    request->callback.act      = loadfiles;
    request->callback.userdata = table;
}
