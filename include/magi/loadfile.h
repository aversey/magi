#ifndef MAGI_INCLUDED_LOADFILE
#define MAGI_INCLUDED_LOADFILE
/* Simple callback to load files.
 *
 * Generally satisfies task of receiving files.  Fill magi_loadfiles table
 * via magi_loadfiles_add, specifying which file-parameter to load into which
 * path, and what are size limitations for it.  When table is complete, setup
 * your request to use this callback with magi_loadfiles_set.
 *
 * This module is optional.
 */
#include "request.h"


/* Rule of loading single file.
 * There is no need to form or edit it directly. */
typedef struct magi_loadfile {
    const char *name;  /* Form field to load file from. */
    const char *path;  /* Path to load file in. */
    int         max;   /* Limit in bytes. Null means unlimited. */
} magi_loadfile;

/* Table of rules for loading files.
 * Set count and files as null to initialize. */
typedef struct magi_loadfiles {
    int            count;  /* Size of files array.*/
    magi_loadfile *files;  /* Dynamic array of rules to load files. */
} magi_loadfiles;


/* Free memory used by table.  Request using table will become invalid. */
void magi_loadfiles_free(magi_loadfiles *table);

/* Add entity into table.
 * Specify form field to load file from with name,
 * wnated loaction to load file with path,
 * and file size limit in bytes with max (pass null to unlimit). */
void magi_loadfiles_add(magi_loadfiles *table,
                        const char     *name,
                        const char     *path,
                        int             max);


/* Setup request to use loadfiles callback with table. */
void magi_loadfiles_set(magi_request *request, magi_loadfiles *table);


#endif
