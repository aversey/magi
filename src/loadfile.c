#include "loadfile.h"

#include <stdio.h>
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
    table->files[table->count].param_name = name;
    table->files[table->count].location   = path;
    table->files[table->count].maximum    = max;
    table->count++;
}

void magi_loadfiles_destroy(magi_loadfiles *table)
{
    if (!table) {
        return;
    }
    free(table->table);
}

static void loadfiles(magi_file *file,
                      char      *addon,
                      int        addon_len,
                      int        is_addon_last,
                      void      *userdata)
{
    magi_loadfiles *table = userdata;
    int             pos;
    if (!file->file_name || !strcmp(file->file_name, "")) {
        return;
    }
    for (pos = 0; pos != table->count; ++pos) {
        if (!strcmp(table->files[pos].param_name, file->param_name)) {
            static FILE *f = 0;
            static int   unlimited;
            static int   left;
            if (!f) {
                const char * loc = table->files[pos].location;
                f                = fopen(loc, "wb");
                left             = table->files[pos].maximum;
                unlimited        = !left;
            }
            if (unlimited) {
                fwrite(addon, 1, addon_len, f);
            } else {
                int min = left < addon_len ? left : addon_len;
                fwrite(addon, 1, min, f);
                left -= min;
            }
            if (is_addon_last) {
                fclose(f);
                f = 0;
            }
            return;
        }
    }
}

void magi_request_setup_loadfiles(magi_request *request, magi_loadfiles *table)
{
    request->file_callback          = loadfiles;
    request->file_callback_userdata = table;
}
