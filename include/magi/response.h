#ifndef MAGI_INCLUDED_RESPONSE
#define MAGI_INCLUDED_RESPONSE
/** @file response.h
 * @brief General response functionality for magi_request.
 *
 * There are two parts of response, namely header and body.
 * You can directly dive into filling the body, since default headers are set.
 * Defult content-type is XHTML, status is 200 (Ok).
 *
 * @warning Use body related functions only after dealing with headers.
 *          (Since storing possibly large body in memory is a bad idea,
 *           all headers should be sent before anything from the body.)
 */
#include "request.h"


void magi_response_status(magi_request *r, magi_status code);
void magi_response_status_custom(magi_request *r, int code, const char *desc);

void magi_response_cookie(magi_request *r, const char *name, const char *data);
void magi_response_cookie_complex(magi_request *r, magi_cookie *c);
void magi_response_cookie_discard(magi_request *r, const char *name);

void magi_response_content_length(magi_request *r, int length);
void magi_response_content_type(magi_request *r, const char *type);

void magi_response(magi_request *r, const char *addon);
void magi_response_format(magi_request *r, const char *format, ...);
void magi_response_file(magi_request *r, FILE *file);


#endif
