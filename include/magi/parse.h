#ifndef MAGI_INCLUDED_PARSE
#define MAGI_INCLUDED_PARSE
/* Common Gateway Interface request parsing implementation
 */
#include "request.h"


/* Analyses non-post part of request (everything except body and files)
 * from environment.  True if ok. */
int magi_parse_head(struct magi_request *request);

/* Complete request with post body from standard input.  True if ok. */
int magi_parse_body(struct magi_request *request);

/* Shortcut for analysing both head and body of request.  True if ok. */
int magi_parse(struct magi_request *request);


#endif
