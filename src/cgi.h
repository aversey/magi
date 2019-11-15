#ifndef MAGI_INCLUDED_CGI
#define MAGI_INCLUDED_CGI

#include "field.h"
#include "request.h"


/*
 * Constructs request using environment variables and standart I/O;
 * Returns null if succeed, otherwise error code.
 */
int magi_cgi(struct magi_request * request,
             /* Callback will be used only for fields loaded via multipart. */
             /* Null callback disables callback system. */
             void (*callback)(struct magi_field * field, char * buffer,
                              int len),
             int max_post);


#endif
