#ifndef MAGI_INCLUDED_PARAM
#define MAGI_INCLUDED_PARAM
/** @file param.h
 * @brief blah...
 *
 * blah-blah...
 */


typedef struct magi_param {
    /* All pointers must be valid as 'free' arguments. */
    char *name;
    char *data;
} magi_param;

typedef struct magi_param_list {
    struct magi_param_list *next; /* Must be valid as 'free' argument. */
    magi_param              item;
} magi_param_list;


/* Addition of item to top of list. Null <=> error. */
int magi_param_list_add(magi_param_list **list,
                        magi_param       *item);

/* Data of the first node in list: node.name == name; else null. */
char *magi_param_list_get(magi_param_list *list, const char *name);

/* Freeing and invalidation of list. */
void magi_param_list_destroy(magi_param_list *list);


#endif
