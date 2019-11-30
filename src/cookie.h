#ifndef MAGI_INCLUDED_COOKIE
#define MAGI_INCLUDED_COOKIE


struct magi_cookie {
    /* All pointers must be valid as 'free' arguments. */
    char * name;
    char * data;
    char * path;
    char * domain;
    char * port;
};

struct magi_cookie_list {
    struct magi_cookie_list * next; /* Must be valid as 'free' arguments. */
    struct magi_cookie        item;
};


/* Addition of item to top of list. Null <=> error. */
int magi_cookie_list_add(struct magi_cookie_list ** list,
                         struct magi_cookie *       item);

/* Data of the first node in list: node.name == name; else null. */
char * magi_cookie_list_get(struct magi_cookie_list * list, const char * name);

/* Freeing and invalidation of list. */
void magi_cookie_list_destroy(struct magi_cookie_list * list);


#endif
