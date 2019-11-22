#ifndef MAGI_INCLUDED_REQUEST
#define MAGI_INCLUDED_REQUEST

#include "cookie.h"
#include "error.h"
#include "file.h"
#include "param.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Request
 *
 * Can be generated via CGI handler (magi_cgi_request) or
 * in a Fast CGI session (magi_fcgi_request).
 *
 *
 * Request URL: http[s]://{server_name}[:{server_port}]{uri}
 *                         example.com    80
 * uri: {document_uri}[?{urlencode(url_params)}]
 *                       alfar=9973&veles=on
 * document_uri: {script_name}{path_info}
 *                /bin/script  /article/magic
 */
struct magi_request {
    /* * * Results * * */

    /* Parsed */
    struct magi_cookie_list * cookies;
    struct magi_param_list *  http_params; /* HTTP Header parameters */
    struct magi_param_list *  url_params;  /* Urlencoded paramteres from URL */
    struct magi_param_list *  params;      /* Parameters from 'post' body */
    struct magi_file_list *   files;       /* Files loaded via multipart */

    /* Environment Shortcuts */
    char * method;          /* REQUEST_METHOD */
    char * uri;             /* REQUEST_URI */
    char * document_root;   /* DOCUMENT_ROOT */
    char * document_uri;    /* DOCUMENT_URI */
    char * script_name;     /* SCRIPT_NAME */
    char * script_filename; /* SCRIPT_FILENAME */
    char * remote_addr;     /* REMOTE_ADDR */
    char * remote_port;     /* REMOTE_PORT */
    char * server_addr;     /* SERVER_ADDR */
    /* Following can be not a domain name, even if request is done with it.
     * (Use http_params["HTTP_HOST"] instead.) */
    char * server_name;     /* SERVER_NAME */
    char * server_port;     /* SERVER_PORT */
    char * server_protocol; /* SERVER_PROTOCOL */
    char * server_software; /* SERVER_COFTWARE */
    char * path_info;       /* PATH_INFO */

    /* Request Error Code */
    enum magi_error error;


    /* * * Internal Data (no need for user to analyse) * * */

    /* Callback for processing files */
    void (*file_callback)(struct magi_file * file,
                          char *             addon,
                          int                addon_len,
                          int                is_addon_last,
                          void *             userdata);
    void * file_callback_userdata;
    int    file_callback_addon_len_max;

    /* Limit for memory used (disables with zero) */
    int max_cookies_size;
    int max_http_params_size;
    int max_params_size;
};


/* Setup request with default settings. */
void magi_request_setup(struct magi_request * request);


/* Destroys request. */
void magi_request_destroy(struct magi_request * request);


#endif
