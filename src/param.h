#ifndef MAGI_INCLUDED_PARAM
#define MAGI_INCLUDED_PARAM


struct magi_param {
    /* All pointers must be valid as 'free' arguments. */
    char * name;
    char * data;
};

struct magi_param_list {
    struct magi_param_list * next; /* Must be valid as 'free' argument. */
    struct magi_param        item;
};


/* Addition of item to top of list. Null <=> error. */
int magi_param_list_add(struct magi_param_list ** list,
                        struct magi_param *       item);

/* Data of the first node in list: node.name == name; else null. */
char * magi_param_list_get(struct magi_param_list * list, const char * name);

/* Freeing and invalidation of list. */
void magi_param_list_destroy(struct magi_param_list * list);


#endif
