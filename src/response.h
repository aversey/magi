#ifndef MAGI_INCLUDED_RESPONSE
#define MAGI_INCLUDED_RESPONSE

#include "cookie.h"
#include "param.h"


typedef struct magi_response {
    magi_cookie_list *cookies;
    magi_param_list  *http_params;
    char             *content_type;
    char             *content;
    int               len;
    int               size;
} magi_response;


void magi_response_setup(magi_response *response);


void magi_response_content_type(magi_response *response, const char *type);

void magi_response_add(magi_response *response, const char *addon);
void magi_response_add_format(magi_response *response, const char *addon, ...);

void magi_response_cookie(magi_response *response, magi_cookie *cookie);

void magi_response_cookie_easy(magi_response *response,
                               const char    *name,
                               const char    *value);

void magi_response_cookie_discard(magi_response *response,
                                  const char    *name);

void magi_response_http(magi_response *response,
                        const char    *name,
                        const char    *data);


void magi_response_destroy(magi_response *response);


#endif
