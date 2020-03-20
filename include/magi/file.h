#ifndef MAGI_INCLUDED_FILE
#define MAGI_INCLUDED_FILE
/* Form field of file with its parameters.
 */
#include "param.h"


typedef struct magi_file {
    char        *field;     /* Name of form field. */
    char        *filename;  /* File name on user's computer. */
    magi_params *params;    /* Multipart params (e.g. type). */
} magi_file;

/* Form files collection, implemented as a linked list. */
typedef struct magi_files {
    struct magi_files *next;  /* Pointer to next files. */
    magi_file          item;  /* File on top. */
} magi_files;


/* Free memory used by files. */
void magi_files_free(magi_files *files);

/* Add newitem onto top of files. */
void magi_files_add(magi_files **files, magi_file *newitem);

/* Get first from top of files file with name, null if no such file. */
const magi_file *magi_files_get(const magi_files *files, const char *name);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Callback to load files while analysing request.
 * addon_len is not null if something to add is in addon
 * and null if file_to_add_into is ended.
 * Files are passed sequentialy, one by one. */
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
