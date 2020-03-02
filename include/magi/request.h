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


typedef struct magi_request_meta {
    char *uri;              /**< REQUEST_URI */
    char *document_root;    /**< DOCUMENT_ROOT */
    char *document_uri;     /**< DOCUMENT_URI */
    char *script_name;      /**< SCRIPT_NAME */
    char *script_filename;  /**< SCRIPT_FILENAME */
    char *remote_addr;      /**< REMOTE_ADDR */
    char *remote_port;      /**< REMOTE_PORT */
    char *addr;             /**< SERVER_ADDR */
    char *name;             /**< SERVER_NAME */
    char *port;             /**< SERVER_PORT */
    char *protocol;         /**< SERVER_PROTOCOL */
    char *software;         /**< SERVER_COFTWARE */
} magi_request_meta;

typedef struct magi_request_config {
} magi_request_config;

typedef struct magi_request_response {
} magi_request_response;

typedef struct magi_request_methods {
} magi_request_methods;

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
    magi_cookies *cookies;
    magi_params  *params_head;  /**< Paramteres from URL */
    magi_params  *params_body;  /**< Parameters from 'post' body */
    magi_files   *files;        /**< 'Post' multipart files */

    magi_request_meta meta;
    char *method;  /**< REQUEST_METHOD */
    char *path;    /**< PATH_INFO */
    int   is_https;
    magi_error error;

    magi_request_config   config;
    magi_request_response response;
    magi_request_methods *methods;
} magi_request;


void magi_request_init(magi_request *request);
void magi_request_free(magi_request *request);


char *magi_request_param(magi_request *request, const char *name);
char *magi_request_param_url(magi_request *request, const char *name);

magi_file *magi_request_file(magi_request *request, const char *name);

char        *magi_request_cookie(magi_request *request, const char *name);
magi_cookie *magi_request_cookie_complex(magi_request *request,
                                         const char *name);


#endif
