#ifndef MAGI_INCLUDED_FILE
#define MAGI_INCLUDED_FILE

#include "param.h"


struct magi_file {
    /* All pointers must be valid as 'free' arguments. */
    char *                   param_name; /* Name of corresponding form field */
    char *                   file_name;  /* File name on user's computer */
    struct magi_param_list * params;     /* Multipart params (e.g. type) */
};

struct magi_file_list {
    struct magi_file_list * next; /* Must be valid as 'free' argument. */
    struct magi_file        item;
};


/* Addition of item to top of list. Null <=> error. */
int magi_file_list_add(struct magi_file_list ** list, struct magi_file * item);

/* First node in list: node.param_name == name; else null. */
struct magi_file * magi_file_list_get(struct magi_file_list * list,
                                      const char *            name);

/* Freeing and invalidation of list. */
void magi_file_list_destroy(struct magi_file_list * list);


#endif
