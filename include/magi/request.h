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

typedef void (*magi_file_callback_act)(void *, magi_file *, char *, int);
typedef struct magi_file_callback {
    magi_file_callback_act act;
    void                  *userdata;
    int                    addon_max;
} magi_file_callback;

typedef struct magi_request_limits {
    int cookies;
    int params_http;
    int params_head;
    int params_body;
} magi_request_limits;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ /**
 * Request and response handler.
 *
 * Can be created via 'magi_request_{gateway interface name}', but will have
 * nullified 'post'-related fields ('params' & 'files').  Reason is unlimited
 * 'post' body size, with possible dependence of wanted limits from data of
 * headers (e.g. session id from cookies, enabling some users to load more).
 * To proceed 'post' use 'magi_request_resume_{gateway interface name}',
 * specifying settings if necessary. */
typedef struct magi_request {
    magi_error  error;

    magi_cookies *cookies;
    magi_params  *meta;
    magi_params  *head;
    magi_params  *body;
    magi_files   *files;

    magi_method method;
    int         is_secure;
    char       *domain;
    int         port;
    char       *script;
    char       *path;

    magi_file_callback  callback;
    magi_request_limits limits;

    struct magi_response_implementation *response;
} magi_request;


void magi_request_init(magi_request *r);
void magi_request_free(magi_request *r);


char *magi_request_meta(magi_request *r, magi_meta code);
char *magi_request_meta_custom(magi_request *r, const char *name);

char *magi_request_param(magi_request *r, const char *name);
char *magi_request_urlparam(magi_request *r, const char *name);

magi_file *magi_request_file(magi_request *r, const char *name);

char        *magi_request_cookie(magi_request *r, const char *name);
magi_cookie *magi_request_cookie_complex(magi_request *r, const char *name);


#endif
