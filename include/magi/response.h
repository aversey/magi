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
#include <stdio.h>
#include <stdarg.h>


/* Response headers as three sequential groups. */
typedef struct magi_response {
    magi_params *head_response;
    magi_params *head_general;
    magi_params *head_entity;
} magi_response;


/* Response initialiser, setup defaults. */
void magi_response_init(magi_response *r);
/* Send response headers. */
void magi_response_send(magi_response *r);
/* Free memory used by response headers. */
void magi_response_free(magi_response *r);

/* Just response defaults. (text/html, 200 Ok) */
void magi_response_default();


/* Change resposne status header. */
void magi_response_status(magi_response *r, int code, const char *description);

/* Add cookie to response. */
void magi_response_cookie(magi_response *r, const char *n, const char *d);
/* Add cookie with additional information to response. */
void magi_response_cookie_complex(magi_response *r, magi_cookie *c);
/* Add request to discard cookie to response. */
void magi_response_cookie_discard(magi_response *r, const char *name);

/* Just add some general custom header. */
void magi_response_header(magi_response *r, const char *n, const char *d);

/* Change Content-Length header. */
void magi_response_content_length(magi_response *r, int length);
/* Change Content-Type header. */
void magi_response_content_type(magi_response *r, const char *type);


#endif
