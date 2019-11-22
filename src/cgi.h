#ifndef MAGI_INCLUDED_CGI
#define MAGI_INCLUDED_CGI

#include "request.h"


/* Constructs request from environment and standard input;
 * Returns null only in case of error. */
int magi_request_cgi(struct magi_request * request);


/* Sends response to standard output;
 * Returns null only in case of error. */
int magi_response_cgi(struct magi_response * response);


#endif
