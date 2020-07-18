#ifndef MAGI_INCLUDED_COOKIE
#define MAGI_INCLUDED_COOKIE
/* HTTP Cookie
 * Described in RFC 6265.
 */


struct magi_cookie {
    char *name;     /* Cookie name. */
    char *data;     /* Cookie value. */
    char *path;     /* Path on which cookie is set, without '/' at the end. */
    char *domain;   /* Domain in wich cookie is set.
                     * With dot at the begining. */
    char *max_age;  /* In seconds until discard (response only). */
};

/* HTTP cookies collection, implemented as a linked list. */
struct magi_cookies {
    struct magi_cookies *next;  /* Pointer to next cookies. */
    struct magi_cookie   item;  /* Cookie on top. */
};


/* Free memory used by cookies. */
void magi_cookies_free(struct magi_cookies *cookies);

/* Add newitem onto top of cookies. */
void magi_cookies_add(struct magi_cookies     **cookies,
                      const struct magi_cookie *newitem);

/* Get first cookie with given name, null if no such cookie.
 * First cookie is the most accurate in terms of domain and path. */
const struct magi_cookie *magi_cookies_get(const struct magi_cookies *cookies,
                                           const char                *name);


#endif
