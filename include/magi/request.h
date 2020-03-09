#ifndef MAGI_INCLUDED_REQUEST
#define MAGI_INCLUDED_REQUEST
/** @file request.h
 * @brief blah...
 *
 * blah-blah...
 */
#include "cookie.h"
#include "error.h"
#include "file.h"
#include "param.h"


typedef struct magi_request_limits {
    int cookies;
    int params_meta;
    int params_head;
    int params_body;
} magi_request_limits;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ /**
 * Request handler.
 *
 * Can be created via 'magi_{gateway interface name}_head', but will have
 * nullified 'POST'-related fields ('params' & 'files').  Reason is unlimited
 * 'POST' body size, with possible dependence of wanted limits from data of
 * headers (e.g. session id from cookies, enabling some users to load more).
 * To proceed 'post' use 'magi_{gateway interface name}_body', specifying
 * settings if necessary.
 *
 * Or just use shortcut 'magi_{gateway interface_name}' to do both parts. */
typedef struct magi_request {
    magi_error  error;

    magi_cookies *cookies;
    magi_params  *meta;
    magi_params  *head;
    magi_params  *body;
    magi_files   *files;

    char *document_root;
    char *method;
    int   is_secure;
    char *host;
    int   port;
    char *script;
    char *path;

    magi_file_callback  callback;
    magi_request_limits limits;

    struct magi_response_implementation *response;
} magi_request;


void magi_request_init(magi_request *r);
void magi_request_free(magi_request *r);


char *magi_request_meta(magi_request *r, const char *name);

char *magi_request_param(magi_request *r, const char *name);
char *magi_request_urlparam(magi_request *r, const char *name);

magi_file *magi_request_file(magi_request *r, const char *name);

char        *magi_request_cookie(magi_request *r, const char *name);
magi_cookie *magi_request_cookie_complex(magi_request *r, const char *name);


#endif
