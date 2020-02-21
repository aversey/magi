#ifndef MAGI_INCLUDED_LOADFILE
#define MAGI_INCLUDED_LOADFILE

#include "request.h"


typedef struct magi_loadfile {
    const char *param_name; /* Form field name, in which file is expected. */
    const char *location;   /* Location to load file in. */
    int         maximum;    /* Limit in bytes. Null <=> unlimited. */
} magi_loadfile;

typedef struct magi_loadfiles {
    int            count;
    magi_loadfile *files;
} magi_loadfiles;

void magi_loadfiles_add(magi_loadfiles *table,
                        const char     *name,
                        const char     *path,
                        int             max);

void magi_loadfiles_destroy(magi_loadfiles *table);

/* Setup request callback with files loaded into corresponding to their
 * parameter names locations; paths are in magi_loadfiles struct. */
void magi_request_setup_loadfiles(magi_request   *request,
                                  magi_loadfiles *table);


#endif
