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


typedef struct magi_response {
    magi_params *head_response;
    magi_params *head_general;
    magi_params *head_entity;
} magi_response;


void magi_response_init(magi_response *r);
void magi_response_free(magi_response *r);

void magi_response_default();


void magi_response_status(magi_response *r, int code, const char *description);

void magi_response_cookie(magi_response *r, const char *n, const char *d);
void magi_response_cookie_complex(magi_response *r, magi_cookie *c);
void magi_response_cookie_discard(magi_response *r, const char *name);

void magi_response_header(magi_response *r, const char *n, const char *d);

void magi_response_content_length(magi_response *r, int length);
void magi_response_content_type(magi_response *r, const char *type);


#endif
