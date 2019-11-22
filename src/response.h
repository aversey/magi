#ifndef MAGI_INCLUDED_RESPONSE
#define MAGI_INCLUDED_RESPONSE

#include "error.h"


enum magi_content_type { magi_xhtml };

struct magi_response {
    struct magi_cookie_list * cookies;
    struct magi_param_list *  http_params;
    struct magi_param_list *  url_params;
    char *                    content_type;
    char *                    content;
};

void magi_response_content_type(struct magi_response * response,
                                enum magi_content_type type);

void magi_reponse_add(struct magi_response * response, const char * addon);
void magi_reponse_add_format(struct magi_response * response,
                             const char *           addon,
                             ...);

void magi_response_cookie(struct magi_response *     response,
                          const struct magi_cookie * cookie);

void magi_response_cookie_easy(struct magi_response * response,
                               const char *           name,
                               const char *           value);

void magi_response_cookie_delete(struct magi_response * response,
                                 const char *           name);


#endif
