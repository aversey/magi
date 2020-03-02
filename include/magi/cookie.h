#ifndef MAGI_INCLUDED_COOKIE
#define MAGI_INCLUDED_COOKIE
/** @file cookie.h
 * @brief HTTP Cookie.
 *
 * Described in RFC 6265.
 */


/** HTTP Cookie. */
typedef struct magi_cookie {
    char *name;     /**< Cookie name. */
    char *data;     /**< Cookie value. */
    char *path;     /**< Path on wich cookie is set.
                     * Without '/' at the end. */
    char *domain;   /**< Domain in wich cookie is set.
                     * With dot at the begining. */
    char *max_age;  /**< In seconds until discard (response only). */
} magi_cookie;

/** HTTP cookies collection.
 * Implemented as a linked list. */
typedef struct magi_cookies {
    struct magi_cookies *next;  /**< Pointer to next cookies. */
    magi_cookie          item;  /**< Cookie on top. */
} magi_cookies;

/** Add @p newitem to @p cookies.
 * @param[in,out] cookies to add into.
 * @param[in] newitem to add onto top of @p cookies. */
void magi_cookies_add(magi_cookies **cookies, magi_cookie *newitem);

/** Get data of cookie from @p cookies with @p name.
 * @note Cookies in @p cookies are in reverse request order, and first cookie
 *       from request is the most accurate in terms of domain and path.
 * @param[in] params to search in.
 * @param[in] name of needed parameter.
 * @return data of the last from top of @p cookies cookie with @p name,
 *         null only if no such parameter. */
char *magi_cookies_get(magi_cookies *cookies, const char *name);

/** Free memory used by @p cookies.
 * @param[in,out] cookies to be destructed. */
void magi_cookies_free(magi_cookies *cookies);


#endif
