#ifndef MAGI_INCLUDED_FILE
#define MAGI_INCLUDED_FILE

#include "param.h"


typedef struct magi_file {
    /* All pointers must be valid as 'free' arguments. */
    char            *param_name; /* Name of corresponding form field */
    char            *file_name;  /* File name on user's computer */
    magi_param_list *params;     /* Multipart params (e.g. type) */
} magi_file;

typedef struct magi_file_list {
    struct magi_file_list *next; /* Must be valid as 'free' argument. */
    magi_file              item;
} magi_file_list;


/* Addition of item to top of list. Null <=> error. */
int magi_file_list_add(magi_file_list **list, magi_file *item);

/* First node in list: node.param_name == name; else null. */
magi_file *magi_file_list_get(magi_file_list *list, const char *name);

/* Freeing and invalidation of list. */
void magi_file_list_destroy(magi_file_list *list);


#endif
