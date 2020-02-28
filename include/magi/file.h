#ifndef MAGI_INCLUDED_FILE
#define MAGI_INCLUDED_FILE
/** @file file.h
 * @brief Form field of file with its parameters.
 */
#include "param.h"


/** @brief Form field of file with its parameters. */
typedef struct magi_file {
    char        *field;     /**<@brief Name of form field. */
    char        *filename;  /**<@brief File name on user's computer. */
    magi_params *params;    /**<@brief Multipart params (e.g. type). */
} magi_file;

/** @brief Form files collection.
 *
 * Implemented as a linked list. */
typedef struct magi_files {
    struct magi_files *next;  /**<@brief Pointer to next files. */
    magi_file          item;  /**<@brief File on top. */
} magi_files;


/** @brief Add @p newitem to @p files.
 * @param[in,out] files to add into.
 * @param[in] newitem to add onto top of @p files. */
void magi_files_add(magi_files **files, magi_file *newitem);

/** @brief Get file with @p name from @p files.
 * @param[in] files to search in.
 * @param[in] name of needed file.
 * @return first from top of @p files file with @p name,
           null only if no such file. */
magi_file *magi_files_get(magi_files *files, const char *name);

/** @brief Free memory used by @p files.
 * @param[in,out] files to be destructed. */
void magi_files_free(magi_files *files);


#endif
