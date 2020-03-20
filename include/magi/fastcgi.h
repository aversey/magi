#ifndef MAGI_INCLUDED_FASTCGI
#define MAGI_INCLUDED_FASTCGI
/* Fast CGI implementation
 */
#include "session.h"
#include "request.h"


int magi_fastcgi_head(magi_session *s, magi_request *r);
int magi_fastcgi_body(magi_session *s, magi_request *r);
int magi_fastcgi(magi_session *s, magi_request *r);


#endif
