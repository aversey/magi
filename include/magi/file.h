#ifndef MAGI_INCLUDED_FILE
#define MAGI_INCLUDED_FILE
/** @file file.h
 * @brief Form field of file with its parameters.
 */
#include "param.h"


/** Form field of file with its parameters. */
typedef struct magi_file {
    char        *field;     /**< Name of form field. */
    char        *filename;  /**< File name on user's computer. */
    magi_params *params;    /**< Multipart params (e.g. type). */
} magi_file;

/** Form files collection.
 * Implemented as a linked list. */
typedef struct magi_files {
    struct magi_files *next;  /**< Pointer to next files. */
    magi_file          item;  /**< File on top. */
} magi_files;


/** Add @p newitem to @p files.
 * @param[in,out] files to add into.
 * @param[in] newitem to add onto top of @p files. */
void magi_files_add(magi_files **files, magi_file *newitem);

/** Get file with @p name from @p files.
 * @param[in] files to search in.
 * @param[in] name of needed file.
 * @return first from top of @p files file with @p name,
 *         null only if no such file. */
magi_file *magi_files_get(magi_files *files, const char *name);

/** Free memory used by @p files.
 * @param[in,out] files to be destructed. */
void magi_files_free(magi_files *files);


typedef void (*magi_file_callback_act)(void *userdata,
                                       magi_file *file_to_add_into,
                                       char *addon,
                                       int addon_len);
typedef struct magi_file_callback {
    magi_file_callback_act act;
    void                  *userdata;
    int                    addon_max;
} magi_file_callback;


#endif
