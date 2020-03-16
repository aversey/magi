#ifndef MAGI_INCLUDED_REQUEST
#define MAGI_INCLUDED_REQUEST
/* Request handler
 *
 * Can be created via magi_{gateway interface name}_head, but will have
 * nullified POST-related fields (params & files).  Reason is unlimited
 * POST body size, with possible dependence of wanted limits from data of
 * headers (e.g. session id from cookies, enabling some users to load more).
 * To proceed POST use magi_{gateway interface name}_body, specifying
 * settings if necessary.
 * Or just use shortcut magi_{gateway interface name} to do both parts.
 */
#include "cookie.h"
#include "error.h"
#include "file.h"
#include "param.h"


/* Limits on possibly enormous structures.  Null means unlimited. */
typedef struct magi_request_limits {
    int cookies;
    int params_meta;
    int params_head;
    int params_body;
} magi_request_limits;

typedef struct magi_request {
    magi_error  error;

    magi_cookies *cookies;  /* Passed HTTP cookies. */
    magi_params  *meta;     /* Request parameters. */
    magi_params  *head;     /* Form field values from URL. */
    magi_params  *body;     /* Form field values from body. */
    magi_files   *files;    /* Form field files metadatas. */

    char *document_root;  /* Server's document root (e.g. /var/www/htdocs). */
    char *method;         /* Request method (GET, HEAD, POST, etc...). */
    int   is_secure;      /* Means requested via HTTPS standard port (443). */
    char *host;           /* Host name (e.g. example.com). */
    int   port;           /* Port (e.g. 80 or 443). */
    char *script;         /* Script path in URL (e.g. /cgi-bin/script). */
    char *path;           /* Path requested for the script (e.g. /login). */
    /* URL has form 'http[s]://{host}:{port}{script}{path}'. */

    magi_file_callback  callback;  /* Callback to actually load files. */
    magi_request_limits limits;

    /* Implementation of response for this request.
     * Especially helpful in case of FastCGI,
     * when you can handle many requests simultaniously,
     * and need to know how to response each of them. */
    struct magi_response_implementation *response;
} magi_request;


/* Request initialiser, setup defaults. */
void magi_request_init(magi_request *r);
/* Free memory used by request. */
void magi_request_free(magi_request *r);


/* Get value of meta-param with name. */
char *magi_request_meta(magi_request *r, const char *name);

/* Get value of form field param (prioritising body) with name. */
char *magi_request_param(magi_request *r, const char *name);
/* Get value of form field param with name from url. */
char *magi_request_urlparam(magi_request *r, const char *name);

/* Get metadata structure of file from file field with name. */
magi_file *magi_request_file(magi_request *r, const char *name);

/* Get value of cookie with name. */
char        *magi_request_cookie(magi_request *r, const char *name);
/* Get cookie with name. */
magi_cookie *magi_request_cookie_complex(magi_request *r, const char *name);


#endif
