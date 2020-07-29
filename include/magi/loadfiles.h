#ifndef MAGI_INCLUDED_LOADFILE
#define MAGI_INCLUDED_LOADFILE
/* Simple callback to load files.
 *
 * Generally satisfies task of receiving files.  Fill magi_loadfiles table
 * via magi_loadfiles_add, specifying which file-parameter to load into which
 * path, and what are size limitations for it.  When table is complete, setup
 * your request to use this callback with magi_loadfiles_set.
 */
#include "request.h"


/* Rule of loading single file.
 * There is no need to form or edit it directly. */
struct magi_loadfile {
    const char *name;  /* Form field to load file from. */
    const char *path;  /* Path to load file in. */
    int         max;   /* Limit in bytes. Null means unlimited. */
};

/* Table of rules for loading files as linked list. */
struct magi_loadfiles {
    struct magi_loadfile  *item;
    struct magi_loadfiles *next;
};


/* Initialise loadfiles. */
void magi_loadfiles_init(struct magi_loadfiles **loadfiles);
/* Free memory used by loadfiles.  Request using table will become invalid. */
void magi_loadfiles_free(struct magi_loadfiles **loadfiles);

/* Add entity into loadfiles.
 * Specify form field to load file from with name,
 * wnated loaction to load file with path,
 * and file size limit in bytes with max (pass null to unlimit). */
void magi_loadfiles_add(struct magi_loadfiles **loadfiles,
                        const char             *name,
                        const char             *path,
                        int                     max);


/* Setup request to use loadfiles callback. */
void magi_loadfiles_set(struct magi_request    *request,
                        struct magi_loadfiles **loadfiles);


#endif
