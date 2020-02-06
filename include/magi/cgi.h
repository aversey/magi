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


/** @brief Analyses non-post part of request from environment.
 * @return Returns null only in case of error. */
int magi_request_cgi_head(magi_request *request);

/** @brief Complete request with post data from standard input.
 * @return Returns null only in case of error. */
int magi_request_cgi_body(magi_request *request);

/** @brief Shortcut for analysing both head and body of request. */
int magi_request_cgi(magi_request *request);


/** @brief Sends response to standard output and destroys it.
 * @return Returns null only in case of error. */
int magi_response_cgi(magi_response *response);


/** @brief Sends a standard response of Bad Request error.
 * @return Returns null only in case of error. */
int magi_error_cgi(magi_error error);


#endif
