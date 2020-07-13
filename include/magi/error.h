#ifndef MAGI_INCLUDED_ERROR
#define MAGI_INCLUDED_ERROR
/* Error codes and messages
 * You can output default error message via magi_error_response.
 */


/* Magi error codes. */
enum magi_error {
    magi_error_none = 0,   /* No error, all is ok. */
    magi_error_nobound,    /* No boundary provided for multipart/form-data. */
    magi_error_unknown,    /* Unknown Content Type. */
    magi_error_notype,     /* No Content Type provided. */
    magi_error_length,     /* Wrong Content Length. */
    magi_error_cookies,    /* Bad Cookies header. */
    magi_error_urlenc,     /* Wrong url encoding. */
    magi_error_multipart,  /* Malformed multipart/form-data. */
    magi_error_limit       /* One of specified limits reached. */
};

/* Get description message of given error. */
const char *magi_error_message(enum magi_error error);


/* Response default error page for given error. */
void magi_error_response(enum magi_error error);


#endif
