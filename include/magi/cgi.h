#ifndef MAGI_INCLUDED_CGI
#define MAGI_INCLUDED_CGI
/* Common Gateway Interface implementation
 */
#include "request.h"


/* Analyses non-post part of request from environment.  True if ok. */
int magi_cgi_head(magi_request *request);

/* Complete request with post body from standard input.  True if ok. */
int magi_cgi_body(magi_request *request);

/* Shortcut for analysing both head and body of request.  True if ok. */
int magi_cgi(magi_request *request);


#endif
