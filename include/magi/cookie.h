#ifndef MAGI_INCLUDED_COOKIE
#define MAGI_INCLUDED_COOKIE
/* HTTP Cookie
 * Described in RFC 6265.
 */


typedef struct magi_cookie {
    char *name;     /* Cookie name. */
    char *data;     /* Cookie value. */
    char *path;     /* Path on which cookie is set.  Without '/' at the end. */
    char *domain;   /* Domain in wich cookie is set.
                     * With dot at the begining. */
    char *max_age;  /* In seconds until discard (response only). */
} magi_cookie;

/* HTTP cookies collection, implemented as a linked list. */
typedef struct magi_cookies {
    struct magi_cookies *next;  /* Pointer to next cookies. */
    magi_cookie          item;  /* Cookie on top. */
} magi_cookies;


/* Free memory used by cookies. */
void magi_cookies_free(magi_cookies *cookies);

/* Add newitem onto top of cookies. */
void magi_cookies_add(magi_cookies **cookies, magi_cookie *newitem);

/* Get first cookie with given name, null if no such cookie.
 * First cookie is the most accurate in terms of domain and path. */
const magi_cookie *magi_cookies_get(const magi_cookies *cookies,
                                    const char *name);


#endif
