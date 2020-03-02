#ifndef MAGI_INCLUDED_LOADFILE
#define MAGI_INCLUDED_LOADFILE
/** @file loadfile.h
 * @brief Simple callback to load files.
 *
 * Generally satisfies task of receiving files.  Fill #magi_loadfiles table
 * via #magi_loadfiles_add, specifying which file-parameter to load into which
 * path, and what are size limitations for it.  When table is complete setup
 * your request to use this callback with #magi_request_setup_loadfiles.
 *
 * @note This module is optional.
 */
#include "request.h"


/** Rule of loading single file.
 * There is no need to form or edit it directly. */
typedef struct magi_loadfile {
    const char *name;  /**< Form field to load file from. */
    const char *path;  /**< Path to load file in. */
    int         max;   /**< Limit in bytes. Null means unlimited. */
} magi_loadfile;

/** Table of rules for loading files.
 * Set @c count and @c files as null to initialize. */
typedef struct magi_loadfiles {
    int            count;  /**< Size of @c files.*/
    magi_loadfile *files;  /**< Dynamic array of rules to load files. */
} magi_loadfiles;


/** Add entity into @p table.
 * @param[in,out] table is the table to add into.
 * @param[in] name is the form field name to load file from.
 * @param[in] path to load file in.
 * @param[in] max is limit in bytes (give null to unlimit). */
void magi_loadfiles_add(magi_loadfiles *table,
                        const char     *name,
                        const char     *path,
                        int             max);

/** Free memmory used by @p table.
 * @warning Request using @p table will become invalid.
 * @param[in,out] table to be destructed. */
void magi_loadfiles_free(magi_loadfiles *table);

/** Setup @p request to use loadfiles callback with @p table.
 * @param[in,out] request to setup using loadfiles callback.
 * @param[in] table to use in loadfiles callback. */
void magi_loadfiles_set(magi_request *request, magi_loadfiles *table);


#endif
