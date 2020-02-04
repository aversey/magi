#ifndef MAGI_INCLUDED_COOKIE
#define MAGI_INCLUDED_COOKIE


typedef struct magi_cookie {
    /* All pointers must be valid as 'free' arguments. */
    char *name;
    char *data;
    char *path;    /* Without '/' at the end. */
    char *domain;  /* With dot at the begining. */
    char *max_age; /* In seconds until discard, used only in response. */
} magi_cookie;

typedef struct magi_cookie_list {
    struct magi_cookie_list *next; /* Must be valid as 'free' arguments. */
    magi_cookie              item;
} magi_cookie_list;


/* Addition of item to top of list. Null <=> error. */
int magi_cookie_list_add(magi_cookie_list **list, magi_cookie *item);

/* Data of last cookie in list: cookie.name == name, returns null if no such;
 * Last cookie in list is first from request, and from RFC 2109 we know that
 * first cookie is the most accurate for request's domain and path. */
char *magi_cookie_list_get(magi_cookie_list *list, const char *name);

/* Freeing and invalidation of list. */
void magi_cookie_list_destroy(magi_cookie_list *list);


#endif
