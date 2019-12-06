#ifndef MAGI_INCLUDED_RESPONSE
#define MAGI_INCLUDED_RESPONSE

#include "cookie.h"
#include "param.h"


enum magi_content_type { magi_xhtml = 0 };

struct magi_response {
    struct magi_cookie_list * cookies;
    struct magi_param_list *  http_params;
    char *                    content_type;
    char *                    content;
    int                       len;
    int                       size;
};


void magi_response_setup(struct magi_response * response);


void magi_response_content_type(struct magi_response * response,
                                enum magi_content_type type);

void magi_response_add(struct magi_response * response, const char * addon);
void magi_response_add_format(struct magi_response * response,
                              const char *           addon,
                              ...);

void magi_response_cookie(struct magi_response * response,
                          struct magi_cookie *   cookie);

void magi_response_cookie_easy(struct magi_response * response,
                               const char *           name,
                               const char *           value);

void magi_response_cookie_discard(struct magi_response * response,
                                  const char *           name);

void magi_response_http(struct magi_response * response,
                        const char *           name,
                        const char *           data);


void magi_response_destroy(struct magi_response * response);


#endif
