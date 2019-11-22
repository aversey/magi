#ifndef MAGI_INCLUDED_COOKIE
#define MAGI_INCLUDED_COOKIE


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Cookie
 */
struct magi_cookie {
    char * name;
    char * data;
    char * path;
    char * domain;
    char * port;
};

struct magi_cookie_list {
    struct magi_cookie_list * next;
    struct magi_cookie        item;
};


/* Returns null only in case of error. */
int magi_cookie_list_add(struct magi_cookie_list ** list,
                         struct magi_cookie *       item);

/* Searchs for first node in list: node.name == name, name is C-string;
 * Returns node.data if succeed, otherwise result is null. */
char * magi_cookie_list_get(struct magi_cookie_list * list, const char * name);

/* Destroys list. */
void magi_cookie_list_free(struct magi_cookie_list * list);


#endif
