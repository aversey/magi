#ifndef MAGI_INCLUDED_CGI
#define MAGI_INCLUDED_CGI
/** @file cgi.h
 * @brief blah...
 *
 * blah-blah...
 */
#include "request.h"


/** Analyses non-post part of request from environment.
 * @return 1 if ok, 0 if error. */
int magi_cgi_head(magi_request *request);

/** Complete request with post body from standard input.
 * @return 1 if ok, 0 if error. */
int magi_cgi_body(magi_request *request);

/** Shortcut for analysing both head and body of request.
 * @return 1 if ok, 0 if error. */
int magi_cgi(magi_request *request);


#endif
