#ifndef MAGI_INCLUDED_CGI
#define MAGI_INCLUDED_CGI
/** @file cgi.h
 * @brief blah...
 *
 * blah-blah...
 */
#include "error.h"
#include "request.h"
#include "response.h"


/** Analyses non-post part of request from environment.
 * @return 1 if ok, 0 if error. */
int magi_request_cgi_head(magi_request *request);

/** Complete request with post body from standard input.
 * @return 1 if ok, 0 if error. */
int magi_request_cgi_body(magi_request *request);

/** Shortcut for analysing both head and body of request.
 * @return 1 if ok, 0 if error. */
int magi_request_cgi(magi_request *request);


/** Sends response to standard output and destroys it.
 * @return 1 if ok, 0 if error. */
int magi_response_cgi(magi_response *response);


/** Sends a standard response of Bad Request error.
 * @return 1 if ok, 0 if error. */
int magi_error_cgi(magi_error error);


#endif
