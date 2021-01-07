#ifndef MAGI_INCLUDED_REQUEST
#define MAGI_INCLUDED_REQUEST
/* Request handler
 *
 * Can be created via magi_parse_head, but will have nullified POST-related
 * fields (params & files).  Reason is unlimited POST body size,
 * with possible dependence of wanted limits from data of headers
 * (e.g. session id from cookies, enabling some users to load more); and also
 * dependence of place to load files by callback from head data.
 * To proceed POST use magi_parse_body, specifying settings if necessary.
 * Or just use shortcut magi_parse to do both parts.
 */
#include "cookie.h"
#include "error.h"
#include "file.h"
#include "param.h"


/* Limits on possibly enormous structures.  Null means unlimited. */
struct magi_request_limits {
    int cookies;
    int params_head;
    int params_body;
    int read_buffer;
};

struct magi_request {
    enum magi_error error;

    struct magi_cookies *cookies;  /* Passed HTTP cookies. */
    struct magi_params  *head;     /* Form field values from URL. */
    struct magi_params  *body;     /* Form field values from body. */
    struct magi_files   *files;    /* Form field files metadatas. */

    char *document_root;  /* Server's document root (e.g. /var/www/htdocs). */
    char *method;         /* Request method (GET, HEAD, POST, etc...). */
    int   is_secure;      /* Means requested via HTTPS standard port (443). */
    char *host;           /* Host name (e.g. example.com). */
    int   port;           /* Port (e.g. 80 or 443). */
    char *script;         /* Script path in URL (e.g. /cgi-bin/script). */
    char *path;           /* Path requested for the script (e.g. /login). */
    /* URL has form 'http[s]://{host}:{port}{script}{path}'. */

    struct magi_file_callback  callback;  /* Callback to load files. */
    struct magi_request_limits limits;
};


/* Request initialiser, setup defaults. */
void magi_request_init(struct magi_request *r);
/* Free memory used by request. */
void magi_request_free(struct magi_request *r);


/* Get value of form field param (prioritizing body) with name. */
char *magi_request_param(const struct magi_request *r, const char *name);
/* Get value of form field param with name from url. */
char *magi_request_urlparam(const struct magi_request *r, const char *name);

/* Get metadata structure of file from file field with name. */
const struct magi_file *magi_request_file(const struct magi_request *r,
                                          const char *name);

/* Get value of cookie with name. */
char *magi_request_cookie(const struct magi_request *r, const char *name);
/* Get cookie with name. */
const struct magi_cookie *
magi_request_cookie_complex(const struct magi_request *r, const char *name);


#endif
