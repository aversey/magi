#ifndef MAGI_INCLUDED_REQUEST
#define MAGI_INCLUDED_REQUEST

#include "cookie.h"
#include "error.h"
#include "file.h"
#include "param.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Request
 *
 * Can be created via 'magi_request_{gateway interface name}', but will have
 * nullified 'post'-related fields ('params' & 'files').  Reason is unlimited
 * 'post' body size, with possible dependence of wanted limits from data of
 * headers (e.g. session id from cookies, enabling some users to load more).
 * To proceed 'post' use 'magi_request_resume_{gateway interface name}',
 * specifying settings if necessary.
 *
 *
 * Cheatsheet on environment:
 *
 * Request URL: http[s]://{server_name}[:{server_port}]{uri}
 *                         example.com    80
 * uri: {document_uri}[?{urlencode(url_params)}]
 *                       alfar=9973&veles=on
 * document_uri: {script_name}{path_info}
 *                /bin/script  /article/magic
 */
typedef struct magi_request {
    /* * * Results * * */
    /* All pointers of this section must be valid as 'free' arguments. */

    /* Parsed */
    magi_cookie_list *cookies;
    magi_param_list  *http_params; /* HTTP Header parameters */
    magi_param_list  *url_params;  /* Paramteres from URL */
    magi_param_list  *params;      /* Parameters from 'post' body */
    magi_file_list   *files;       /* 'Post' multipart files */

    /* Environment Shortcuts */
    char *method;          /* REQUEST_METHOD */
    char *uri;             /* REQUEST_URI */
    char *document_root;   /* DOCUMENT_ROOT */
    char *document_uri;    /* DOCUMENT_URI */
    char *script_name;     /* SCRIPT_NAME */
    char *script_filename; /* SCRIPT_FILENAME */
    char *remote_addr;     /* REMOTE_ADDR */
    char *remote_port;     /* REMOTE_PORT */
    char *server_addr;     /* SERVER_ADDR */
    char *server_name;     /* SERVER_NAME */
    /* server_name can be not a domain name, even if request is done with it.
     * (Use http_params["HTTP_HOST"] instead.) */
    char *server_port;     /* SERVER_PORT */
    char *server_protocol; /* SERVER_PROTOCOL */
    char *server_software; /* SERVER_COFTWARE */
    char *path_info;       /* PATH_INFO */

    /* Request Error Code */
    magi_error error;


    /* * * Settings * * */

    /* Callback for processing files */
    void (*file_callback)(magi_file *file,
                          char      *addon,
                          int        addon_len,
                          int        is_addon_last,
                          void      *userdata);
    void *file_callback_userdata;
    int   file_callback_addon_max;

    /* Limits for memory used (null <=> unlimitted) */
    int cookies_max;
    int url_params_max;
    int http_params_max;
    int params_max;
} magi_request;


/* Setup request with default settings. */
void magi_request_setup(magi_request *request);


/* Destroys request. */
void magi_request_destroy(magi_request *request);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Tempfiles Callback
 */
typedef struct magi_tempfile {
    const char *param_name; /* Form field name, in which file is expected. */
    const char *location;   /* Location to load file in. */
    int         maximum;    /* Null <=> unlimited. */
} magi_tempfile;

typedef struct magi_tempfiles {
    int            count;
    magi_tempfile *tmps;
} magi_tempfiles;

void magi_tempfiles_add(magi_tempfiles *tmps,
                        const char     *name,
                        const char     *path,
                        int             max);

void magi_tempfiles_destroy(magi_tempfiles *tmps);

/* Setup request callback with files loaded into corresponding to their
 * parameter names locations; paths are in magi_tempfiles struct. */
void magi_request_setup_tempfiles(magi_request   *request,
                                  magi_tempfiles *table);


#endif
