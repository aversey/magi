#include "request.h"

#include "cookie.h"
#include "field.h"
#include "param.h"
#include <stdlib.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Common Request Handling
 */
void magi_request_destroy(struct magi_request * request)
{
    if (request) {
        magi_field_list_destroy(request->fields);
        magi_cookie_list_destroy(request->cookies);
        free(request->fields);
        free(request->cookies);
        free(request->method);
        free(request->uri);
        free(request->document_root);
        free(request->document_uri);
        free(request->script_name);
        free(request->script_filename);
        free(request->remote_addr);
        free(request->remote_port);
        free(request->server_addr);
        free(request->server_name);
        free(request->server_port);
        free(request->server_protocol);
        free(request->server_software);
        magi_param_list_destroy(request->http_params);
        free(request->http_params);
    }
}
