#ifndef MAGI_INCLUDED_REQUEST_H
#define MAGI_INCLUDED_REQUEST_H

#include "field.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Request
 */
struct magi_request {
    struct magi_field_list  *fields;
    struct magi_cookie_list *cookies;
    char                    *method;
    char                    *uri;
    char                    *document_root;
    char                    *document_uri;
    char                    *script_name;
    char                    *script_filename;
    char                    *remote_addr;
    char                    *remote_port;
    char                    *server_addr;
    char                    *server_name;
    char                    *server_port;
    char                    *server_protocol;
    char                    *server_software;
    struct magi_param_list  *http_params;
};  /* http[s]://{server_name}[:{server_port}]{script_name}[?...] */


/* Common Request Handling */
/*
 * Destroys request; request is not valid after destruction.
 */
void magi_request_destroy(struct magi_request *request);

/* CGI Request Handling */
/*
 * Constructs request using environment variables and standart I/O;
 * Returns null if succeed, otherwise error code.
 */
int magi_request_build_cgi(
    struct magi_request *request,
    /* Callback will be used only for fields loaded via multipart. */
    /* Null callback disables callback system. */
    void (*callback)(struct magi_field *field, char *buffer, int len),
    int max_post
);

/* TODO: FastCGI Request Handling */


#endif
