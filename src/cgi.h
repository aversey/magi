#ifndef MAGI_INCLUDED_CGI
#define MAGI_INCLUDED_CGI

#include "error.h"
#include "request.h"
#include "response.h"


/* Constructs non-post part of request from environment;
 * Returns null only in case of error. */
int magi_request_cgi(struct magi_request * request);

/* Complete request with post data from standard input;
 * Returns null only in case of error. */
int magi_request_resume_cgi(struct magi_request * request);


/* Sends response to standard output;
 * Returns null only in case of error. */
int magi_response_cgi(struct magi_response * response);

/* Sends a standart response of Bad Request error to standard output;
 * Returns null only in case of error. */
int magi_error_cgi(enum magi_error error);


#endif
