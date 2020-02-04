#ifndef MAGI_INCLUDED_CGI
#define MAGI_INCLUDED_CGI

#include "error.h"
#include "request.h"
#include "response.h"


/* Constructs non-post part of request from environment;
 * Returns null only in case of error. */
int magi_request_cgi(magi_request *request);

/* Complete request with post data from standard input;
 * Returns null only in case of error. */
int magi_request_resume_cgi(magi_request *request);

/* Shortcut for applying both previous. */
int magi_request_full_cgi(magi_request *request);


/* Sends response to standard output;
 * Returns null only in case of error. */
int magi_response_cgi(magi_response *response);

/* Shortcut for magi_response_cgi with desctruction after it. */
int magi_response_cgi_clear(magi_response *response);


/* Sends a standart response of Bad Request error to standard output;
 * Returns null only in case of error. */
int magi_error_cgi(magi_error error);


#endif
