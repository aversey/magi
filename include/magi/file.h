#ifndef MAGI_INCLUDED_FILE
#define MAGI_INCLUDED_FILE
/* Form field of file with its parameters.
 */
#include "param.h"


struct magi_file {
    char               *field;     /* Name of form field. */
    char               *filename;  /* File name on user's computer. */
    struct magi_params *params;    /* Multipart params (e.g. type). */
};

/* Form files collection, implemented as a linked list. */
struct magi_files {
    struct magi_files *next;  /* Pointer to next files. */
    struct magi_file   item;  /* File on top. */
};


/* Free memory used by files. */
void magi_files_free(struct magi_files *files);

/* Add newitem onto top of files. */
void magi_files_add(struct magi_files     **files,
                    const struct magi_file *newitem);

/* Get first from top of files file with name, null if no such file. */
const struct magi_file *magi_files_get(const struct magi_files *files,
                                       const char              *name);


/* Get value of parameter with name from file->params. */
char *magi_file_param(struct magi_file *file, const char *name);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Callback to load files while analysing request.
 * addon_len is not null if something to add is in addon
 * and null if file is ended.
 * newfile flag is setted up in the beginning of new file.
 * Files are passed sequentialy, one by one. */
typedef void (*magi_file_callback_act)(void                   *userdata,
                                       int                     newfile,
                                       const struct magi_file *file,
                                       const char             *addon,
                                       int                     addon_len);

struct magi_file_callback {
    magi_file_callback_act act;
    void                  *userdata;
    int                    addon_max;
};


#endif
