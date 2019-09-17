#ifndef MAGI_INCLUDED_COOKIE
#define MAGI_INCLUDED_COOKIE


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Cookie
 */
struct magi_cookie {
    char *name;    /* name: free(name) is valid. */
    char *data;    /* data: free(data) is valid. */
    /* Following is used in Cookie2: */
    char *path;    /* path: free(path) is valid. */
    char *domain;  /* domain: free(domain) is valid. */
    char *port;    /* port: free(port) is valid. */
};

/* Null is valid "struct magi_cookie_list *" object. */
struct magi_cookie_list {
    struct magi_cookie_list *next;
    struct magi_cookie      item;
};

/* Returns null in case of error. */
int magi_parse_cookie(struct magi_cookie_list **list, const char *input);

/*
 * Adds *item to the begining of *list, item and list are dereferencable;
 * Returns null in case of error.
 */
int magi_cookie_list_add(
    struct magi_cookie_list **list,
    struct magi_cookie *item
);

/*
 * Searchs for first node in list: node.name == name, name is C-string;
 * Returns node.data if succeed, otherwise result is null.
 */
char *magi_cookie_list_get(struct magi_cookie_list *list, const char *name);

/*
 * Destroys list; list is not valid after destruction.
 */
void magi_cookie_list_destroy(struct magi_cookie_list *list);


#endif
