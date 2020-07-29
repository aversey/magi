#ifndef MAGI_INCLUDED_RESPONSE
#define MAGI_INCLUDED_RESPONSE
/* General CGI headers response functionality.
 *
 * There are two parts of response, namely head and body.
 * You need to output head first, it is done with magi_response_free or with
 * magi_response_default.  Default content-type is HTML, status is 200 (Ok).
 * Body can be outputed after head into stdout.
 */
#include "cookie.h"
#include "param.h"


/* Response headers as three sequential groups. */
struct magi_response {
    struct magi_params *head_response;
    struct magi_params *head_general;
    struct magi_params *head_entity;
};


/* Response initialiser, setup defaults. */
void magi_response_init(struct magi_response *r);
/* Send response headers. */
void magi_response_send(const struct magi_response *r);
/* Free memory used by response headers. */
void magi_response_free(struct magi_response *r);

/* Just response defaults. (text/html, 200 Ok) */
void magi_response_default();


/* Change resposne status header. */
void magi_response_status(struct magi_response *r,
                          int                   code,
                          const char           *description);

/* Add cookie to response. */
void magi_response_cookie(struct magi_response *r,
                          const char           *name,
                          const char           *data);
/* Add cookie with additional information to response. */
void magi_response_cookie_complex(struct magi_response     *r,
                                  const struct magi_cookie *c);
/* Add request to discard cookie to response. */
void magi_response_cookie_discard(struct magi_response *r,
                                  const char           *name);

/* Just add some general custom header. */
void magi_response_header(struct magi_response *r,
                          const char           *name,
                          const char           *data);

/* Change Content-Length header. */
void magi_response_content_length(struct magi_response *r, int length);
/* Change Content-Type header. */
void magi_response_content_type(struct magi_response *r, const char *type);


#endif
